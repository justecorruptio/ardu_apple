import random
from PIL import Image, ImageFilter, ImageOps

WANT_HEIGHT = 64
WANT_WIDTH = 68
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

def tween(prev_data, data):
    blocks = []
    errors = 0
    for y in xrange(0, H, BS):
        for x in xrange(0, W, BS):
            diff = 0
            for dy in xrange(y, y+BS):
                if dy >= H:
                    break
                for dx in xrange(x, x+BS):
                    if dx >= W:
                        break
                    if data[dx + dy * W] != prev_data[dx + dy * W]:
                        diff += 1

            if random.randint(0, 100) < 0:
            #if errors < 5:
                allowable = 1
            else:
                allowable = 0

            if diff <= allowable:
                errors += 1
                for dy in xrange(y, y+BS):
                    if dy >= H:
                        break
                    for dx in xrange(x, x+BS):
                        if dx >= W:
                            break
                        data[dx + dy * W] = prev_data[dx + dy * W]

            blocks.append(int(diff <= allowable))

    return blocks

def rle_h(data, blocks):

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

    #if len(ret) >= 64:
    #    ret = simplify(ret)

    #if len(ret) >= 63:
    #    raise Exception("TOO LONG")

    return ret

def rle_v(data, blocks):

    cur_col = 0
    rl = 0
    ret = []
    for i in xrange(len(data)):
        x, y = i / H, i % H
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
    #    ret = simplify(ret)

    #if len(ret) >= 63:
    #    raise Exception("TOO LONG")

    return ret

def rle_b(data, blocks):

    cur_col = 0
    rl = 0
    ret = []
    b = 4
    for i in xrange(len(data)):
        xb = (i / (b*b)) % (W/b)
        yb = (i / (b*b)) / (W/b)

        x = xb * b + (i % b)
        y = yb * b + ((i % (b*b)) / b)

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

    return ret

def simplify(data):
    if len(data) < 3:
        return data

    ret = [data[0]]
    i = 1
    while i < len(data) - 1:
        if data[i] == 1 and data[i + 1] != 1 and data[i - 1] != 1:
            ret[-1] += data[i + 1] + 1
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
        #if d > 255:
        #    raise Exception("TOOOO LARGE:", d)
        if d > 15:
            nibs.append(0)
            nibs.append((d & 0xf))
            d = d >> 4
            while d > 0x7:
                nibs.append((d & 0x7) | 0x8)
                d = d >> 3
            nibs.append(d & 0x7)
        else:
            nibs.append(d)

    return tl, nibs

def encode(blocks, rh, rv):
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
        # (tl, nibs), horiz, block_enc
        (to_nibs(rh), True, False),
        (to_nibs(rv), False, False),
    ]
    all_nibs.sort(key=lambda x: len(x[0][1]))

    (tl, nibs), horiz, block_enc = all_nibs[0]

    #if len(nibs) >= 255:
    #    raise Exception("TOO LONG")

    len_nibs = len(nibs)

    # FORCE HORIZ FOR NIBS > 63
    #byts.append( (int(horiz) << 6) | (tl << 7) | len_nibs )
    byts.append( len_nibs & 0xff)
    byts.append( (len_nibs >> 8) & 0xff)
    byts[-1] |= (int(horiz) << 6) | (tl << 7)

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

            rv = rle_v(data, blocks)
            rh = rle_h(data, blocks)

            d = encode(blocks, rh, rv)
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

#process(40, 6523)
process(40, 1000)
#process(475, 482)

'''
prev = get_frame(1)
im = get_frame(1182)
#im.show()
b, c = tween(prev, im)
#print b, c
d = encode(b, c)
'''
