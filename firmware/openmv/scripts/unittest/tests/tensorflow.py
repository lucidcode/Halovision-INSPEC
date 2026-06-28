def unittest(data_path, temp_path):
    import omv

    if "MPS3" in omv.arch():
        return "skip"

    import ml
    import image
    from ml.postprocessing.edgeimpulse import Fomo

    img = image.Image(data_path + "/faces.bmp", copy_to_fb=True)
    # FOMO requires square input, so crop the center of the image.
    s = min(img.width(), img.height())
    img = img.crop(roi=((img.width() - s) // 2, (img.height() - s) // 2, s, s))
    model = ml.Model(data_path + "/fomo_face_detection.tflite", postprocess=Fomo(threshold=0.4))
    output = model.predict([img])

    return output[1] == [
        ([130, 85, 31, 31], 0.9375),
        ([85, 17, 31, 31], 0.875),
        ([130, 198, 31, 31], 0.4257813),
    ]
