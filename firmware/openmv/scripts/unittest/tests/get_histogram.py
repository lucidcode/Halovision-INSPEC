def unittest(data_path, temp_path):
    import image

    e = 0.0000001
    # Load image
    img = image.Image(data_path + "/cat.pgm", copy_to_fb=True)

    # Load histogram
    with open(data_path + "/cat.csv", "r") as f:
        hist1 = [float(x) for x in f.read().split(",")]

    # Get histogram
    hist2 = img.get_histogram()

    # Compare
    for a, b in zip(hist1, hist2[0]):
        if abs(a - b) > e:
            return False

    return hist2.get_percentile(0.5)[0] == 96 and hist2.get_statistics()[0:] == (
        # mean, median, mode, stdev, min, max, lq, uq
        81, 96, 0, 59, 0, 255, 13, 128,
        # l_mean, l_median, l_mode, l_stdev, l_min, l_max, l_lq, l_uq
        81, 96, 0, 59, 0, 255, 13, 128,
        # a_mean, a_median, a_mode, a_stdev, a_min, a_max, a_lq, a_uq
        0, 0, 0, 0, 0, 0, 0, 0,
        # b_mean, b_median, b_mode, b_stdev, b_min, b_max, b_lq, b_uq
        0, 0, 0, 0, 0, 0, 0, 0,
    )
