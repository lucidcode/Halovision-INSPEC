def unittest(data_path, temp_path):
    import image
    import time

    img = image.Image(data_path + "/shapes.ppm", copy_to_fb=True)

    total = 0
    iterations = 100
    for _ in range(iterations):
        start = time.ticks_us()
        rects = img.find_rects(threshold=50000)
        total += time.ticks_diff(time.ticks_us(), start)
    print("find_rects: %d us avg (%d runs)" % (total // iterations, iterations))

    return len(rects) == 1 and rects[0][0:] == (23, 39, 35, 36, 146566)


temp_path = "/remote/temp"
data_path = "/remote/data"

if __name__ == "__main__":
    unittest(data_path, temp_path)
