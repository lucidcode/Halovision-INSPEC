def unittest(data_path, temp_path):
    import omv

    if "MPS3" not in omv.arch():
        return "skip"

    import ml
    import image
    from ml.preprocessing import Normalization
    from ml.postprocessing.mediapipe import BlazeFace
    from ml.postprocessing.mediapipe import FaceLandmarks

    img = image.Image(data_path + "/faces.bmp", copy_to_fb=True)
    # BlazeFace requires square input, so crop the center of the image.
    s = min(img.width(), img.height())
    img = img.crop(roi=((img.width() - s) // 2, (img.height() - s) // 2, s, s))

    # First detect a face.
    face_detection = ml.Model(data_path + "/blazeface_front_128.tflite", postprocess=BlazeFace(threshold=0.4))
    faces = face_detection.predict([img])

    expected_rects = [
        ([76, 14, 48, 48], 0.796841),
        ([163, 155, 36, 36], 0.7004726),
        ([170, 2, 40, 40], 0.5),
        ([4, 97, 36, 36], 0.5),
    ]

    if len(faces) != len(expected_rects):
        return False
    for i, (rect, score, _kp) in enumerate(faces):
        if rect != expected_rects[i][0]:
            return False

    # Use the first detected face for landmarks.
    r, score, keypoints = faces[0]
    wider_rect = (r[0] - r[2] // 2, r[1] - r[3] // 2, r[2] * 2, r[3] * 2)
    n = Normalization(roi=wider_rect)

    face_landmarks = ml.Model(data_path + "/face_landmarks_192.tflite", postprocess=FaceLandmarks(threshold=0.4))
    marks = face_landmarks.predict([n(img)])

    if len(marks) != 1:
        return False

    r, score, keypoints = marks[0]
    if r != [74, 0, 50, 61]:
        return False
    if keypoints.shape != (468, 3):
        return False
    return True
