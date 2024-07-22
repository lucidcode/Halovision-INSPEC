# This file is part of the OpenMV project.
#
# Copyright (c) 2013-2023 Ibrahim Abdelkader <iabdalkader@openmv.io>
# Copyright (c) 2013-2023 Kwabena W. Agyeman <kwagyeman@openmv.io>
#
# This work is licensed under the MIT license, see the file LICENSE for details.

import errno
import image
import random
import re
import socket
import struct
import time


class rtsp_server:
    def __valid_tcp_socket(self):  # private
        if self.__tcp__socket is None:
            try:
                s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                try:
                    if hasattr(socket, "SO_REUSEADDR"):
                        s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
                    s.bind(self.__myaddr)
                    s.listen(0)
                    s.settimeout(5)
                    self.__tcp__socket, self.__client_addr = s.accept()
                except OSError:
                    pass
                s.close()
            except OSError:
                self.__tcp__socket = None
        return self.__tcp__socket is not None

    def __close_tcp_socket(self):  # private
        if self.__tcp__socket is not None:
            self.__tcp__socket.close()
            self.__tcp__socket = None
        if self.__playing:
            self.__playing = False
            if self.__teardown_cb:
                self.__teardown_cb(self.__pathname, self.__playing_session)

    def __valid_udp_socket(self):  # private
        if self.__udp_rtp__socket is None:
            try:
                self.__udp_rtp__socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
                try:
                    if hasattr(socket, "SO_REUSEADDR"):
                        self.__udp_rtp__socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
                    self.__udp_rtp__socket.bind((self.__myip, self.__client_rtp_port))
                except OSError:
                    self.__udp_rtp__socket.close()
                    self.__udp_rtp__socket = None
            except OSError:
                self.__udp_rtp__socket = None
        if self.__udp_rtcp__socket is None:
            try:
                self.__udp_rtcp__socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
                try:
                    if hasattr(socket, "SO_REUSEADDR"):
                        self.__udp_rtcp__socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
                    self.__udp_rtcp__socket.bind((self.__myip, self.__client_rtcp_port))
                except OSError:
                    self.__udp_rtcp__socket.close()
                    self.__udp_rtcp__socket = None
            except OSError:
                self.__udp_rtcp__socket = None
        return self.__udp_rtp__socket is not None and self.__udp_rtcp__socket is not None

    def __close_udp_socket(self):  # private
        if self.__udp_rtp__socket is not None:
            self.__udp_rtp__socket.close()
            self.__udp_rtp__socket = None
        if self.__udp_rtcp__socket is not None:
            self.__udp_rtcp__socket.close()
            self.__udp_rtcp__socket = None

    def __init__(self, network_if, port=554):  # private
        self.__network = network_if
        self.__myip = self.__network.ifconfig()[0]
        self.__myaddr = (self.__myip, port)
        self.__tcp__socket = None
        self.__udp_rtp__socket = None
        self.__udp_rtcp__socket = None
        self.__setup_cb = None
        self.__play_cb = None
        self.__pause_cb = None
        self.__teardown_cb = None
        self.__pathname = ""
        self.__session = random.getrandbits(30)
        self.__transport_is_tcp = False
        self.__client_rtp_addr = None
        self.__playing = False
        self.__playing_session = 0
        self.__sequence_number = random.getrandbits(16)
        self.__ssrc = random.getrandbits(30)
        print("IP Address:Port %s:%d\nRunning..." % self.__myaddr)

    def register_setup_cb(self, cb):  # public
        self.__setup_cb = cb

    def register_play_cb(self, cb):  # public
        self.__play_cb = cb

    def register_pause_cb(self, cb):  # public
        self.__pause_cb = cb

    def register_teardown_cb(self, cb):  # public
        self.__teardown_cb = cb

    def __send_rtsp_response(self, code, name, extra=""):  # private
        self.__tcp__socket.send("RTSP/1.0 %d %s\r\n%s\r\n" % (code, name, extra))

    def __send_rtsp_response_cseq(self, code, name, seq, extra=""):  # private
        self.__send_rtsp_response(code, name, "CSeq: %d\r\n%s" % (seq, extra))

    def __send_rtsp_response_ok(self, seq, extra=""):  # private
        self.__send_rtsp_response_cseq(200, "OK", seq, extra)

    def __parse_rtsp_request(self, data):  # private
        if data[0] == 0x24:
            return  # Interleaved RTCP Packet
        if data[0] >= 0x80:
            return  # Ressambled TCP
        s = list(
            filter(lambda x: x and not x.startswith("User-Agent"), data.decode().splitlines())
        )
        if s and len(s) >= 2:
            line0 = s[0].split(" ")
            request = line0[0]
            self.__pathname = re.sub("rtsp(u)?://[a-zA-Z0-9\-\.]+(:\d+)?(/)?", "/", line0[1])
            if self.__pathname != "/" and self.__pathname.endswith("/"):
                self.__pathname = self.__pathname[:-1]
            if line0[2] == "RTSP/1.0":
                if len(s) >= 3 and s[1].split(" ")[0] != "CSeq:":
                    for i in range(2, len(s)):
                        if s[i].split(" ")[0] == "CSeq:":
                            temp = s[i]
                            s[i] = s[1]
                            s[1] = temp
                            break
                line1 = s[1].split(" ")
                if line1[0] == "CSeq:":
                    seq = int(line1[1])
                    if request == "OPTIONS":
                        if len(s) >= 3:
                            m = re.match("(Proxy-)?Require: (\S)+", s[2])
                            if m:
                                self.__send_rtsp_response_cseq(
                                    551,
                                    "Option not supported",
                                    seq,
                                    "Unsupported: %s\r\n" % m.group(2),
                                )
                                return
                        self.__send_rtsp_response_ok(
                            seq, "Public: DESCRIBE, SETUP, PLAY, PAUSE, TEARDOWN\r\n"
                        )
                        return
                    elif request == "DESCRIBE":
                        payload = (
                            "v=0\r\no=- %d %d IN IP4 %s\r\ns=OpenMV Video\r\nt=0 0\r\nm=video 0 RTP/AVP 26"
                            % (random.getrandbits(30), random.getrandbits(30), self.__myip)
                        )
                        self.__send_rtsp_response_ok(
                            seq,
                            "Content-Type: application/sdp\r\nContent-Length: %d\r\n\r\n%s"
                            % (len(payload) + 2, payload),
                        )
                        return
                    elif request == "SETUP":
                        if len(s) >= 3:
                            m = re.match(
                                "Transport: RTP/AVP(/UDP)?;unicast;client_port=(\d+)-(\d+)", s[2]
                            )
                            if m:
                                self.__transport_is_tcp = False
                                self.__client_rtp_port = int(m.group(2))
                                self.__client_rtcp_port = int(m.group(3))
                                self.__client_rtp_addr = (
                                    self.__client_addr[0],
                                    self.__client_rtp_port,
                                )
                                self.__client_rtcp_addr = (
                                    self.__client_addr[0],
                                    self.__client_rtcp_port,
                                )
                                self.__session = random.getrandbits(30)
                                self.__ssrc = random.getrandbits(30)
                                self.__valid_udp_socket()
                                self.__send_rtsp_response_ok(
                                    seq,
                                    "%s;server_port=%d-%d;ssrc=%d\r\nSession: %d\r\n"
                                    % (
                                        s[2],
                                        self.__client_rtp_port,
                                        self.__client_rtcp_port,
                                        self.__ssrc,
                                        self.__session,
                                    ),
                                )
                                if self.__setup_cb:
                                    self.__setup_cb(self.__pathname, self.__session)
                                return
                            m = re.match(
                                "Transport: RTP/AVP/TCP;unicast;interleaved=(\d+)-(\d+)", s[2]
                            )
                            if m:
                                self.__transport_is_tcp = True
                                self.__client_rtp_channel = int(m.group(1))
                                self.__client_rtcp_channel = int(m.group(2))
                                self.__session = random.getrandbits(30)
                                self.__ssrc = random.getrandbits(30)
                                self.__send_rtsp_response_ok(
                                    seq, "%s\r\nSession: %d\r\n" % (s[2], self.__session)
                                )
                                if self.__setup_cb:
                                    self.__setup_cb(self.__pathname, self.__session)
                                return
                    elif request == "PLAY":
                        s = list(filter(lambda x: x and not x.startswith("Range"), s))
                        if len(s) >= 3:
                            m = re.match("Session: (\d+)", s[2])
                            if m:
                                self.__playing = True
                                self.__playing_session = int(m.group(1))
                                self.__send_rtsp_response_ok(
                                    seq,
                                    "RTP-Info: url=%s;seq=%d\r\n"
                                    % (line0[1], self.__sequence_number),
                                )
                                if self.__play_cb:
                                    self.__play_cb(self.__pathname, self.__playing_session)
                                return
                    elif request == "PAUSE":
                        if len(s) >= 3:
                            m = re.match("Session: (\d+)", s[2])
                            if m:
                                self.__playing = False
                                self.__send_rtsp_response_ok(seq)
                                if self.__pause_cb:
                                    self.__pause_cb(self.__pathname, int(m.group(1)))
                                return
                    elif request == "TEARDOWN":
                        if len(s) >= 3:
                            m = re.match("Session: (\d+)", s[2])
                            if m:
                                self.__playing = False
                                self.__send_rtsp_response_ok(seq)
                                if self.__teardown_cb:
                                    self.__teardown_cb(self.__pathname, int(m.group(1)))
                                return
                    else:
                        self.__send_rtsp_response_cseq(501, "Not Implemented", seq)
                        return
        self.__send_rtsp_response(400, "Bad Request")

    def __parse_rtcp_packet(self, data):  # private
        pass

    def __valid_socket(self):  # private
        if self.__transport_is_tcp:
            return self.__client_rtp_channel is not None
        else:
            return self.__valid_udp_socket() and self.__client_rtp_addr is not None

    def __settimeout(self, timeout):  # private
        if self.__transport_is_tcp:
            self.__tcp__socket.settimeout(timeout)
        else:
            self.__udp_rtp__socket.settimeout(timeout)
            self.__udp_rtcp__socket.settimeout(timeout)

    def __send(self, data):  # private
        if self.__transport_is_tcp:
            self.__tcp__socket.sendall(
                struct.pack(">BBH", 0x24, self.__client_rtp_channel, len(data)) + data
            )
            return True
        else:
            return self.__udp_rtp__socket.sendto(data, self.__client_rtp_addr)

    def __recv(self):  # private
        if self.__transport_is_tcp:
            return self.__tcp__socket.recv(1400)
        else:
            return self.__udp_rtcp__socket.recv(1400)

    def __close_socket(self):  # private
        if self.__transport_is_tcp:
            self.__close_tcp_socket()
        else:
            self.__close_udp_socket()

    def __send_rtp(self, image_callback, quality):  # private
        img = image_callback(self.__pathname, self.__session)
        img = img.to_jpeg(quality=quality, subsampling=image.JPEG_SUBSAMPLING_422)
        if img.width() >= 2040:
            raise ValueError("Maximum width is 2040")
        if img.height() >= 2040:
            raise ValueError("Maximum height is 2040")
        i = 0
        l = img.size()
        rtp_header_size = 12
        jpeg_header_size = 8
        max_packet_size = 1400 - rtp_header_size - jpeg_header_size
        if self.__transport_is_tcp:
            max_packet_size -= 4
        if self.__valid_socket():
            try:
                self.__settimeout(5)
                timestamp = (time.ticks_ms() * 90) & 0xFFFFFFFF
                mv = memoryview(img)
                while l:
                    rtp_header = struct.pack(
                        ">BBHII",
                        0x80,
                        0x9A if l <= max_packet_size else 0x1A,
                        self.__sequence_number,
                        timestamp,
                        self.__ssrc,
                    )
                    self.__sequence_number = (self.__sequence_number + 1) & 0xFFFF
                    jpeg_header = struct.pack(
                        ">IBBBB", i, 0, quality, int(img.width() // 8), int(img.height() // 8)
                    )
                    img_data = mv[i : i + min(l, max_packet_size)]
                    img_data_len = len(img_data)
                    if not self.__send(rtp_header + jpeg_header + img_data):
                        break
                    i += img_data_len
                    l -= img_data_len
                if l:
                    self.__close_socket()
            except OSError:
                self.__close_socket()
        if not self.__transport_is_tcp and self.__valid_socket():
            try:
                self.__settimeout(0.001)
                try:
                    data = self.__recv()
                    if data and len(data):
                        self.__parse_rtcp_packet(data)
                except OSError as e:
                    if e.errno != errno.EAGAIN and e.errno != errno.ETIMEDOUT:
                        raise e
            except OSError:
                self.__close_socket()

    def stream(self, image_callback, quality=90):  # public
        while True:
            if self.__valid_tcp_socket():
                try:
                    self.__tcp__socket.settimeout(0.001)
                    try:
                        data = self.__tcp__socket.recv(1400)
                        if data and len(data):
                            self.__parse_rtsp_request(data)
                        else:
                            raise OSError
                    except OSError as e:
                        if e.errno != errno.EAGAIN and e.errno != errno.ETIMEDOUT:
                            raise e
                    if self.__playing:
                        self.__send_rtp(image_callback, quality)
                except OSError:
                    self.__close_tcp_socket()
                    self.__close_udp_socket()
