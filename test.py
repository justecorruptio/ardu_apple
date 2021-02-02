import random
random.seed(0)
from PIL import Image, ImageFilter, ImageOps

WANT_HEIGHT = 32
WANT_WIDTH = 34
BS = 8
#MAKE SURE blocks as 2 bits left over

THRESH = 20
JUMP = 6
BW = WANT_WIDTH / BS if WANT_WIDTH % BS == 0 else WANT_WIDTH / BS + 1
BL = 0

W = WANT_WIDTH
H = WANT_HEIGHT


def get_frame(n):
    im = Image.open('data/frames/out%06d.png' % (n,))
    W, H = im.size

    im = ImageOps.grayscale(im)
    im = im.point(lambda v: 0 if v < THRESH else 255)

    #im = im.filter(ImageFilter.GaussianBlur(3))
    im = im.filter(ImageFilter.MaxFilter(5))
    #im = im.filter(ImageFilter.MinFilter(5))

    margin = (W - H * WANT_WIDTH / WANT_HEIGHT) / 2

    im = im.crop((margin, 0, W - margin , H))
    W, H = im.size

    im = im.resize((WANT_WIDTH, WANT_HEIGHT))
    W, H = im.size

    #im = im.convert('1')
    return im, to_data(im)

def to_data(im):
    return map(lambda x: int(x > 0), list(im.getdata()))

def block_unchanged(prev_data, data, x, y): # -> unchanged
    d = [0] * 16
    for dy in xrange(BS):
        if y + dy >= H:
            break
        for dx in xrange(BS):
            if x + dx >= W:
                break
            loc = (x + dx) + (y + dy) * W
            if data[loc] != prev_data[loc]:
               d[
                    int(dx == 0) << 0 |
                    int(dy == 0) << 1 |
                    int(dx == BS - 1) << 2 |
                    int(dy == BS - 1) << 3
                ] += 1

    r = random.randint(0, 100)
    c = d[0b1001] + d[0b1100] + d[0b0110] + d[0b0011]
    e = d[0b0001] + d[0b0010] + d[0b0100] + d[0b1000]
    m = sum(d) - c - e

    return (
        (sum(d) == 0)
        or
        (c == 1 and e == 0 and m == 0 and r < 50)
        or
        (c == 0 and e <= 5 and all(d[i] <= 2 for i in [0b0001, 0b0010, 0b0100, 0b1000]) and m < 8)
        or
        (c == 0 and e == 1 and m < 10)
        #/or (c == 0 and e == 0 and m < 12)
    )

def tween(prev_data, data):
    blocks = []
    for y in xrange(0, H, BS):
        for x in xrange(0, W, BS):

            unchanged = block_unchanged(prev_data, data, x, y)
            if unchanged: # overwrite data
                for dy in xrange(y, y+BS):
                    if dy >= H:
                        break
                    for dx in xrange(x, x+BS):
                        if dx >= W:
                            break
                        data[dx + dy * W] = prev_data[dx + dy * W]

            blocks.append(int(bool(unchanged)))

    return blocks

def rle(data, blocks, xy):
    cur_col = 0
    rl = 0
    ret = []
    for i in xrange(len(data)):
        x, y = xy(i)
        v = data[y * W + x]
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

    #if len(ret) >= 64:
    ret = simplify(ret)

    return ret

def simplify(data):
    if len(data) < 3:
        return data

    ret = [data[0]]
    i = 1
    while i < len(data) - 1:
        if data[i] <=2 and data[i + 1] > 1 and data[i - 1] > 1 and random.randint(0, 100) < 20:
            ret[-1] += data[i + 1] + data[i]
            i += 1
        else:
            ret.append(data[i])
        i += 1

    if i < len(data):
        ret.append(data[i])
    return ret

def to_nibs(data):
    if data and data[0] == 0:
        tl = 1
        data = data[1:]
    else:
        tl = 0
    nibs = []
    for d in data:
        if d == 0:
            raise Exception("Zero width should not happen in RLE")
        if d > 15:
            d -= 16
            nibs.append(0)
            while d > 0x7:
                nibs.append((d & 0x7) | 0x8)
                d = d >> 3
            nibs.append(d & 0x7)
        else:
            nibs.append(d)

    return tl, nibs

def xy_h(i):
    return i % W, i / W

def xy_v(i):
    return i / H, i % H

def xy_hz(i):
    y = i / W
    if y % 2 == 0:
        return i % W, y
    else:
        return W - 1 - i % W, y

def xy_vz(i):
    x = i / H
    if x % 2 == 0:
        return x, i % H
    else:
        return x, H - 1 - i % H

def encode(data, blocks):
    global BL

    byts = []

    v = 0
    flushed = False
    for i, b in enumerate(blocks):
        v |= b << (i % 8)
        flushed = False
        if i % 8 ==  7:
            flushed = True
            byts.append(v)
            v = 0
    if not flushed:
        byts.append(v)

    BL = len(byts)

    all_nibs = [
        # (tl, nibs), horiz, zig
        (to_nibs(rle(data, blocks, xy_h)), True, False),
        (to_nibs(rle(data, blocks, xy_v)), False, False),
        (to_nibs(rle(data, blocks, xy_hz)), True, True),
        (to_nibs(rle(data, blocks, xy_vz)), False, True),
    ]
    all_nibs.sort(key=lambda x: len(x[0][1]))

    (tl, nibs), horiz, zig = all_nibs[0]

    if len(nibs) >= 0x1fff:
        raise Exception("TOO LONG")

    len_nibs = len(nibs)

    # FORCE HORIZ FOR NIBS > 63
    #byts.append( (int(horiz) << 6) | (tl << 7) | len_nibs )
    byts.append( len_nibs & 0xff)
    byts.append( (len_nibs >> 8) & 0x1f)
    byts[-1] |= (int(zig) << 5 | int(horiz) << 6) | (tl << 7)

    if len_nibs % 2 == 1:
        nibs.append(0)
    for i in xrange(0, len(nibs), 2):
        v = nibs[i] | nibs[i + 1] << 4
        byts.append( v )

    return byts



def process(start, frames):
    print "#ifndef FRAMES_H"
    print "#define FRAMES_H"
    print "PROGMEM const uint8_t FRAMES [] = {"

    total = 0
    count = 0
    rows = []
    prev, prev_data = get_frame(1)
    for i in xrange(start + JUMP, frames, JUMP):

        options = []
        for o in xrange(JUMP):
            im, data = get_frame(i + o)
            blocks = tween(prev_data, data)

            d = encode(data, blocks)
            options.append((d, im, data))
        options.sort(key=lambda x: len(x[0]))
        d, im, data = options[0]

        print ','.join('0x%02x' % (v,) for v in d) + ','

        count += 1
        total += len(d)
        prev, prev_data = im, data

    print """ };"""
    print "#define FRAMES_BYTES", total
    print "#define FRAMES_W", WANT_WIDTH
    print "#define FRAMES_H", WANT_HEIGHT
    print "#define FRAMES_BS", BS
    print "#define FRAMES_BW", BW
    print "#define FRAMES_BL", BL
    print "#define FRAMES_JUMP", JUMP
    print "#endif"

process(40, 6523)
#process(40, 1000)
#process(475, 482)

'''
prev = get_frame(1)
im = get_frame(1182)
#im.show()
b, c = tween(prev, im)
#print b, c
d = encode(b, c)
'''
