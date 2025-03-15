import ujson
import utime
import time
import os

class lucid_scribe_data:
    def __init__(self, config):
        self.config = config

        if self.config.get('CreateLogs'):
            self.create_lsd()

    def create_lsd(self):
        self.lsd_hour = 0
        self.lsd_minute = 0
        self.lsd_start = utime.ticks_ms()
        self.lsd_minute_start = self.lsd_start

        root_dir_exists = False
        entries = os.listdir()
        for entry in entries:
            if entry == "sessions":
                root_dir_exists = True

        if not root_dir_exists:
            print("creating sessions")
            os.mkdir("sessions")

        vision_index = 0
        entries = os.listdir("sessions")
        for entry in entries:
            if ("session_" in entry):
                directory_index = entry.replace("session_", "")
                if int(directory_index) >= vision_index:
                    vision_index = int(directory_index) + 1

        self.session_directory = "sessions/" + "session_" + str(vision_index)
        print("creating session " + str(vision_index))
        os.mkdir(self.session_directory)

        config_file = open(self.session_directory + '/config.txt', 'w')
        config_file.write(ujson.dumps(self.config))
        config_file.close()

        self.session_file = self.session_directory + "/session_" + str(vision_index) + ".LSD"
        self.lsd_file = open(self.session_file, 'w')
        self.lsd_file.write("INSPEC")
        self.lsd_file.write("\r\n" + "Researcher:" + self.config.get('Researcher'))
        self.lsd_file.close()
        self.lsd_values = "0"
        self.rem_values = "0"
        self.sqi_values = "0"

    def log(self, variance, rem, sqi):
        if self.config.get('CreateLogs') != 1:
            return

        now = utime.ticks_ms()

        if (now - self.lsd_start > 1000 * 60 * 60):
            self.lsd_start = now
            self.lsd_minute_start = now
            self.lsd_hour = self.lsd_hour + 1
            self.lsd_minute = 0
            self.write_log()
            self.lsd_values = str(variance)
            self.rem_values = str(rem)
            self.sqi_values = str(rem)
            return

        if (now - self.lsd_minute_start >= 1000 * 60):
            self.lsd_minute_start = now
            self.lsd_minute = self.lsd_minute + 1
            self.write_log()
            self.lsd_values = str(variance)
            self.rem_values = str(rem)
            self.sqi_values = str(rem)
            return

        self.lsd_values = f'{self.lsd_values},{str(variance)}'
        self.rem_values = f'{self.rem_values},{str(rem)}'
        self.sqi_values = f'{self.sqi_values},{str(quality)}'

    def write_log(self):
        self.lsd_file = open(self.session_file, 'a')
        formatted_time = self.format_time()
        self.lsd_file.write(f'\r\n{formatted_time}:lsd - {self.lsd_values}')
        self.lsd_file.write(f'\r\n{formatted_time}:rem - {self.rem_values}')
        self.lsd_file.write(f'\r\n{formatted_time}:sqi - {self.sqi_values}')
        self.lsd_file.close()
        
    def add_image(self, image, eye_movements):
        if self.config.get('CreateLogs') != 1:
            return
            
        now = utime.ticks_ms()
        second = time.localtime()[5]
        formatted_time = f'{self.format_time()}:{second}:{eye_movements}'
        formatted_time = formatted_time.replace(":", "-")
        file_name = self.session_directory + "/image_" + formatted_time + ".JPG"
        image.save(file_name)

    def list_directories(self):
        entries = os.listdir("sessions")
        directories = ""
        for entry in entries:
            if ("vision_" in entry):
                if directories == "":
                    directories = entry.replace("session_", "")
                else:
                    directories = f'{directories},{entry.replace("session_", "")}'

        return directories

    def get_config(self, vision):
        file_name = "sessions/session_" + vision + "/config.txt"

        with open(file_name, mode='rb') as file:
            content = bytearray(file.read())
            return content

    def format_time(self):
        hour_string = str(self.lsd_hour)
        if self.lsd_hour < 10:
            hour_string = "0" + hour_string

        minute_string = str(self.lsd_minute)
        if self.lsd_minute < 10:
            minute_string = "0" + minute_string

        return hour_string + ":" + minute_string
