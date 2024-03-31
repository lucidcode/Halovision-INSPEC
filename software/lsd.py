import ujson
import utime
import os

class lucid_scribe_data:
    def __init__(self, config):
        self.config = config
        self.create_lsd()

    def create_lsd(self):
        self.lsd_hour = 0
        self.lsd_minute = 0
        self.lsd_start = utime.ticks_ms()
        self.lsd_minute_start = self.lsd_start

        root_dir_exists = False
        entries = os.listdir()
        for entry in entries:
            if entry == "visions":
                root_dir_exists = True

        if not root_dir_exists:
            print("creating visions")
            os.mkdir("visions")

        vision_index = 0
        entries = os.listdir("visions")
        for entry in entries:
            if ("vision_" in entry):
                directory_index = entry.replace("vision_", "")
                if int(directory_index) >= vision_index:
                    vision_index = int(directory_index) + 1

        self.session_directory = "visions/" + "vision_" + str(vision_index)
        print("creating vision " + str(vision_index))
        os.mkdir(self.session_directory)

        config_file = open(self.session_directory + '/config.txt', 'w')
        config_file.write(ujson.dumps(self.config))
        config_file.close()
        
        self.session_file = self.session_directory + "/session_" + str(vision_index) + ".LSD"
        self.lsd_file = open(self.session_file, 'w')
        self.lsd_file.write("INSPEC")
        self.lsd_file.write("\r\n" + "Researcher:" + self.config['Researcher'])
        self.lsd_file.write("\r\n" + self.format_time() + " - ")
        self.lsd_file.close()
        self.minute_values = "0"

    def log(self, diff):
        now = utime.ticks_ms()

        if (now - self.lsd_start > 1000 * 60 * 60):
            self.lsd_start = now
            self.lsd_minute_start = now
            self.lsd_hour = self.lsd_hour + 1
            self.lsd_minute = 0
            self.write_log()
            self.minute_values = str(diff)
            return

        if (now - self.lsd_minute_start >= 1000 * 60):
            self.lsd_minute_start = now
            self.lsd_minute = self.lsd_minute + 1
            self.write_log()
            self.minute_values = str(diff)
            return

        self.minute_values = f'{self.minute_values},{str(diff)}'

    def write_log(self):
        self.lsd_file = open(self.session_file, 'a')
        self.lsd_file.write(self.minute_values)
        self.lsd_file.write("\r\n" + self.format_time() + " - ")
        self.lsd_file.close()            
        print(self.format_time() + " - " + str(len(self.minute_values)))

    def format_time(self):
        minute_string = str(self.lsd_minute)
        if self.lsd_minute < 10:
            minute_string = "0" + minute_string
            
        hour_string = str(self.lsd_hour)
        if self.lsd_hour < 10:
            hour_string = "0" + hour_string

        return hour_string + ":" + minute_string



        
