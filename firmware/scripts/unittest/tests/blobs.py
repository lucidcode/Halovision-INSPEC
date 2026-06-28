def unittest(data_path, temp_path):
    import image

    thresholds = [
        (0, 100, 56, 95, 41, 74),   # generic_red_thresholds
        (0, 100, -128, -22, -128, 99),  # generic_green_thresholds
        (0, 100, -128, 98, -128, -16),  # generic_blue_thresholds
    ]

    # Load image
    img = image.Image(data_path + "/blobs.ppm", copy_to_fb=True)

    blobs = img.find_blobs(thresholds, pixels_threshold=200, area_threshold=200)

    return (
        blobs[0][0:7] == (61, 21, 49, 41, 84, 41, 1556)
        and blobs[1][0:7] == (22, 20, 39, 45, 40, 42, 1294)
        and blobs[2][0:7] == (105, 20, 36, 41, 124, 38, 1004)
    )
