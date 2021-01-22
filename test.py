from PIL import Image, ImageFilter, ImageOps

WANT_HEIGHT = 16
THRESH = 200
JUMP = 6

def get_frame(n):
    im = Image.open('data/frames/out%06d.png' % (n,))
    W, H = im.size

    im = ImageOps.grayscale(im)
    im = im.point(lambda v: 0 if v < THRESH else 255)

    #im = im.filter(ImageFilter.GaussianBlur(3))
    im = im.filter(ImageFilter.MaxFilter(5))

    im = im.resize((WANT_HEIGHT * W / H, WANT_HEIGHT))
    W, H = im.size

    #im = im.convert('1')
    return im

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

def encode(blocks, data):
    if data and data[0] == 0:
        tl = 1
        data = data[1:]
    else:
        tl = 0
    #print "  TL:", tl
    if data:
        skip = data[0]
        data = data[1:]
    else:
        skip = 0
    #print "SKIP:", skip
    nibs = []
    for d in data:
        while d > 15:
            nibs.extend([15, 0])
            d -= 15
        nibs.append(d)
    print "NIBS:", nibs
    len_nibs = len(nibs)
    #print " LEN:", len_nibs
    #print "BLCK:", blocks

    byts = []
    for i in xrange(3):
        v = 0
        for j in xrange(8):
            v |= blocks[i * 8 + j] << j
        byts.append(v)
    byts.append( (tl << 7) | skip )
    byts.append( len_nibs )

    if len_nibs % 2 == 1:
        nibs.append(0)
    for i in xrange(0, len(nibs), 2):
        v = nibs[i] | nibs[i + 1] << 4
        byts.append( v )

    print "ENCD:", ', '.join(map(hex, byts))
    return byts



def process(start, frames):
    total = 0
    count = 0
    rows = []
    prev = get_frame(start)
    for i in xrange(start + JUMP, frames, JUMP):
        im = get_frame(i)
        b, c = tween(prev, im)
        print i, c, len(c)

        d = encode(b, c)

        count += 1
        total += len(d)
        prev = im

    print "//TOT:", total

#process(1, 6572)
#process(475, 482)

prev = get_frame(1)
im = get_frame(1182)
#im.show()
b, c = tween(prev, im)
print b, c
d = encode(b, c)
