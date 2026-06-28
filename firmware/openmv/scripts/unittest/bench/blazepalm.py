def unittest(data_path, temp_path):
    import gc
    import omv
    import time
    import ml
    import image
    from ml.postprocessing.mediapipe import BlazePalm

    img = image.Image(data_path + "/hand.bmp", copy_to_fb=True)
    model = ml.Model("/rom/palm_detection_full_192.tflite", postprocess=BlazePalm(threshold=0.4))

    total = 0
    iterations = 100
    for i in range(iterations):
        start = time.ticks_us()
        results = model.predict([img])
        total += time.ticks_diff(time.ticks_us(), start)
        if i % 10 == 9:
            gc.collect()
    print("blazepalm predict: %d us avg (%d runs)" % (total // iterations, iterations))

    if len(results) != 1:
        return False
    rect, score, keypoints = results[0]
    if rect != [107, 194, 152, 152]:
        return False
    return True

temp_path = "/remote/temp"
data_path = "/remote/data"

if __name__ == "__main__":
    unittest(data_path, temp_path)
