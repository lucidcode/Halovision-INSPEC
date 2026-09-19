import utime

class rapid_eye_movement:
    def __init__(self, config, face):
        self.config = config
        self.face = face
        self.eye_movements = 0
        self.last_eye_movement = utime.ticks_ms()
        self.artifact_streak = 0.0
        self.last_artifact = utime.ticks_ms()

    def detect(self, variance, global_variance):
        now = utime.ticks_ms()

        if global_variance >= self.config.get('TossThreshold'):
            self.eye_movements = 0
            self.last_eye_movement = now + 1000 * self.config.get('TossCooldown')
            return self.eye_movements

        if now - self.last_eye_movement > 1000 * 60 and self.eye_movements > 0:
            self.eye_movements = self.eye_movements - 1
            self.last_eye_movement = now - 1000 * 58

        if (self.config.get('TrackFace') or self.config.get('TensorFlow') or self.config.get('BlazeFace')) and not self.face.has_face:
            return self.eye_movements

        artifact_filter = self.config.get('ArtifactFilter')
        artifact_variance = variance + variance * (1.0 - artifact_filter)
        outside_variance = global_variance - variance
        if artifact_filter != 0 and global_variance > artifact_variance and outside_variance >= self.config.get('TriggerThreshold'):
            self.artifact_streak += 1
            if artifact_filter >= 0.5:
                if self.artifact_streak > 4:
                    self.eye_movements = 0
                elif self.eye_movements > 0 and now - self.last_artifact > 1000:
                    self.eye_movements -= 1
                    self.last_artifact = now
            return self.eye_movements
        else:
            if self.artifact_streak > 0:
                self.artifact_streak = max(0.0, self.artifact_streak - 0.25)

        if variance < self.config.get('TriggerThreshold'):
            return self.eye_movements

        if now - self.last_eye_movement > 1000:
            self.last_eye_movement = now
            if self.eye_movements < 8:
                self.eye_movements = self.eye_movements + 1

        return self.eye_movements
