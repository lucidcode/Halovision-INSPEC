def unittest(data_path, temp_path):
    import image

    img = image.Image(data_path + "/edl_lines.pgm", copy_to_fb=True)
    lines = img.find_line_segments(threshold=50)

    if len(lines) < 20:
        return False

    # Collect detected angles into 15-degree buckets and track max length per bucket.
    buckets = {}
    for l in lines:
        t = l[0:]
        if len(t) != 8:
            return False
        x1, y1, x2, y2, length, magnitude, theta, rho = t
        if length <= 0 or magnitude <= 0:
            return False
        if theta < 0 or theta >= 180:
            return False
        b = theta // 15
        if b not in buckets or length > buckets[b]:
            buckets[b] = length

    # The test image has lines at many angles. We should detect at least
    # 6 distinct 15-degree angle buckets.
    if len(buckets) < 6:
        return False

    # The test image has long lines (>100px) for horizontal (90), diagonal
    # (45, 135), and the angled lines (~120, ~150). Verify we detect at
    # least 4 segments longer than 80px.
    long_count = sum(1 for v in buckets.values() if v > 80)
    if long_count < 4:
        return False

    return True
