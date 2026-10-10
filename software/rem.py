import utime

class rapid_eye_movement:
    def __init__(self, config, face):
        self.config = config
        self.face = face
        self.eye_movements = 0
        self.last_eye_movement = utime.ticks_ms()
        self.artifact_streak = 0.0
        self.last_artifact = utime.ticks_ms()
        self.last_frame = utime.ticks_ms()
        self.last_eye_frame = self.last_frame - 100000
        self.movement_start = self.last_frame - 100000
        self.previous_start = self.last_frame - 100000

    def detect(self, variance, global_variance, regional_variance=None):
        now = utime.ticks_ms()

        if regional_variance is None:
            regional_variance = variance
        outside_variance = global_variance - regional_variance

        frame_time = min(max(now - self.last_frame, 0), 1000)
        self.last_frame = now

        toss_threshold = self.config.get('TossThreshold')
        is_toss = global_variance >= toss_threshold
        if self.face.has_face:
            is_toss = outside_variance >= toss_threshold or regional_variance >= toss_threshold * 4

        if is_toss:
            self.eye_movements = 0
            self.artifact_streak = 0.0
            self.last_eye_movement = now + 1000 * self.config.get('TossCooldown')
            return self.eye_movements

        if now - self.last_eye_movement > 1000 * 60 and self.eye_movements > 0:
            self.eye_movements = self.eye_movements - 1
            self.last_eye_movement = now - 1000 * 58

        if (self.config.get('TrackFace') or self.config.get('TensorFlow') or self.config.get('BlazeFace')) and not self.face.has_face:
            return self.eye_movements

        artifact_filter = self.config.get('ArtifactFilter')

        artifact_floor = max(self.config.get('TriggerThreshold'), self.config.get('PixelRange'))

        eye_movement = variance >= self.config.get('TriggerThreshold')

        if artifact_filter != 0 and eye_movement and outside_variance > variance * (1.0 - artifact_filter) and outside_variance >= artifact_floor:
            self.artifact_streak += frame_time
            if artifact_filter >= 0.5:
                if self.artifact_streak > self.config.get('ArtifactDuration'):
                    self.eye_movements = 0
                elif self.eye_movements > 0 and now - self.last_artifact > 1000:
                    self.eye_movements -= 1
                    self.last_artifact = now
            return self.eye_movements
        else:
            if self.artifact_streak > 0:
                self.artifact_streak = max(0.0, self.artifact_streak - frame_time / 4)

        if not eye_movement:
            return self.eye_movements

        burst = self.config.get('EyeBurst')
        if burst:
            if now - self.last_eye_frame > 300:
                self.previous_start = self.movement_start
                self.movement_start = now
            self.last_eye_frame = now
            if self.movement_start - self.previous_start > burst:
                return self.eye_movements

        if now - self.last_eye_movement > 1000:
            self.last_eye_movement = now
            if self.eye_movements < 8:
                self.eye_movements = self.eye_movements + 1

        return self.eye_movements
