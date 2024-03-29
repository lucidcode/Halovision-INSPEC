import ujson
import utime

class lucid_scribe_data:
    def __init__(self, config, session_directory):
        self.config = config
        self.session_directory = session_directory
        self.create_lsd()

    def create_lsd(self):
        self.lsd_hour = 0
        self.lsd_minute = 0
        self.lsd_file = open(self.session_directory + "/session.LSD", 'w')
        self.lsd_file.write("INSPEC")
        self.lsd_file.write("\r\n" + "Researcher:" + self.config['Researcher'])
        self.lsd_file.write("\r\n" + "" + str(self.lsd_hour) + ":0" + str(self.lsd_minute) + " - 0")
        self.lsd_start = utime.ticks_ms()
        self.lsd_minute_start = self.lsd_start

    def log(self, diff):
        now = utime.ticks_ms()

        if (now - self.lsd_minute_start >= 1000 * 60):
            self.lsd_minute_start = now
            self.lsd_file.close()
            self.lsd_minute = self.lsd_minute + 1
            self.lsd_file = open(self.session_directory + "/session.LSD", 'a')

            minute_string = str(self.lsd_minute)
            if self.lsd_minute < 10:
                minute_string = "0" + minute_string
            
            self.lsd_file.write("\r\n" + str(self.lsd_hour) + ":" + minute_string + " - " + str(diff))
        else:
            self.lsd_file.write("," + str(diff))

        if (now - self.lsd_start > 1000 * 60 * 60):
            self.lsd_start = now
            self.lsd_hour = self.lsd_hour + 1
            self.lsd_minute = 0

        
