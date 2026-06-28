def unittest(data_path, temp_path):
    import omv

    if "MPS3" not in omv.arch():
        return "skip"

    import ml
    import image
    from ml.postprocessing.mediapipe import BlazeFace

    img = image.Image(data_path + "/faces.bmp", copy_to_fb=True)
    # BlazeFace requires square input, so crop the center of the image.
    s = min(img.width(), img.height())
    img = img.crop(roi=((img.width() - s) // 2, (img.height() - s) // 2, s, s))
    model = ml.Model(data_path + "/blazeface_front_128.tflite", postprocess=BlazeFace(threshold=0.4))
    output = model.predict([img])

    expected = [
        ([76, 14, 48, 48], 0.796841),
        ([163, 155, 36, 36], 0.7004726),
        ([170, 2, 40, 40], 0.5),
        ([4, 97, 36, 36], 0.5),
    ]

    if len(output) != len(expected):
        return False
    for i, (rect, score, _kp) in enumerate(output):
        if rect != expected[i][0]:
            return False
    return True
