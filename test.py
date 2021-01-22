from PIL import Image, ImageFilter, ImageOps

WANT_HEIGHT = 16
THRESH = 200

def get_frame(n):
    #im = Image.open('data/frames/out000327.png')
    im = Image.open('data/frames/out%06d.png' % (n,))
    W, H = im.size

    im = ImageOps.grayscale(im)
    im = im.point(lambda v: 0 if v < THRESH else 255)

    #im = im.filter(ImageFilter.GaussianBlur(3))
    im = im.filter(ImageFilter.MaxFilter(5))

    im = im.resize((WANT_HEIGHT * W / H, WANT_HEIGHT))
    W, H = im.size

    #im = im.convert('1')
    #print list(im.getdata()), len(list(im.getdata()))
    return im

def compress1(im):
    W, H = im.size
    data = map(lambda x: int(x > 0), list(im.getdata()))
    #print data
    cur_col = 0
    rl = 0
    ret = []
    for v in data:
        if v == cur_col:
            rl += 1
        else:
            ret.append(rl)
            rl = 1
            cur_col = 1 - cur_col
    if rl:
        ret.append(rl)
    return ret

def compress2(im):
    BS = 4
    W, H = im.size
    data = map(lambda x: int(x > 0), list(im.getdata()))
    blocks = []
    for y in xrange(0, H, BS):
        for x in xrange(0, W, BS):
            col = data[x + y * W]
            all_same = True
            for dy in xrange(y, y+BS):
                if dy >= H:
                    break
                for dx in xrange(x, x+BS):
                    if dx >= W:
                        break
                    if data[dx + dy * W] != col:
                        all_same = False
            blocks.append(int(all_same) * 2 + int(col))

    BW = W / BS if W % BS ==0 else W / BS + 1

    cur_col = 0
    rl = 0
    ret = []
    for i, v in enumerate(data):
        x, y = i % W, i / W
        if blocks[x / BS + (y / BS) * BW] > 1:
            continue
        if v == cur_col:
            rl += 1
        else:
            ret.append(rl)
            rl = 1
            cur_col = 1 - cur_col
    if rl:
        ret.append(rl)
    ret = ret[:-1]
    return blocks, ret

def tween(prev, im):
    BS = 4
    W, H = im.size
    data = map(lambda x: int(x > 0), list(im.getdata()))
    prev_data = map(lambda x: int(x > 0), list(prev.getdata()))

    blocks = []
    for y in xrange(0, H, BS):
        for x in xrange(0, W, BS):
            all_same = True
            for dy in xrange(y, y+BS):
                if dy >= H:
                    break
                for dx in xrange(x, x+BS):
                    if dx >= W:
                        break
                    if data[dx + dy * W] != prev_data[dx + dy * W]:
                        all_same = False
            blocks.append(int(all_same))

    BW = W / BS if W % BS ==0 else W / BS + 1

    cur_col = 0
    rl = 0
    ret = []
    for i, v in enumerate(data):
        x, y = i % W, i / W
        if blocks[x / BS + (y / BS) * BW] > 0:
            continue
        if v == cur_col:
            rl += 1
        else:
            ret.append(rl)
            rl = 1
            cur_col = 1 - cur_col
    if rl:
        ret.append(rl)
    ret = ret[:-1]
    return blocks, ret


total = 0
count = 0
for i in xrange(1, 6572, 6):
    im = get_frame(i)

    #b, c = compress2(im)
    #print "K", i, c, len(c)
    if i > 1:
        b, c = tween(prev, im)
        print "T", i, c, len(c)

        count += 1
        total += (len(c) - 1.5) / 2
    prev = im


print "AVG:", total * 1. / count
print "EST:", count * ( 1 + 24 / 8) + total

'''
prev = get_frame(4822)
im = get_frame(4828)
b, c = compress2(im)
print "K", c, len(c)
b, c = tween(prev, im)
print "T", c, len(c)

im.show()
'''
