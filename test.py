from PIL import Image, ImageFilter, ImageOps

WANT_HEIGHT = 16
WANT_WIDTH =20
THRESH = 200
JUMP = 6
BS = 4
BW = WANT_WIDTH / BS if WANT_WIDTH % BS == 0 else WANT_WIDTH / BS + 1


def get_frame(n):
    im = Image.open('data/frames/out%06d.png' % (n,))
    W, H = im.size

    im = ImageOps.grayscale(im)
    im = im.point(lambda v: 0 if v < THRESH else 255)

    #im = im.filter(ImageFilter.GaussianBlur(3))
    im = im.filter(ImageFilter.MaxFilter(5))

    margin = (W - H * WANT_WIDTH / WANT_HEIGHT) / 2

    im = im.crop((margin, 0, W - margin , H))
    W, H = im.size

    im = im.resize((WANT_WIDTH, WANT_HEIGHT))
    W, H = im.size

    #im = im.convert('1')
    return im

def tween(prev, im):

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

    #BW = W / BS if W % BS ==0 else W / BS + 1

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
        tl = 0
        data = data[1:]
    else:
        tl = 1
    #print "  TL:", tl
    '''
    if data:
        skip = data[0]
        data = data[1:]
    else:
        skip = 0
    print "SKIP:", skip
    '''
    nibs = []
    for d in data:
        while d > 15:
            nibs.extend([15, 0])
            d -= 15
        nibs.append(d)
    #print "NIBS:", nibs
    len_nibs = len(nibs)
    #print " LEN:", len_nibs
    #print "BLCK:", blocks

    byts = []
    '''
    for i in xrange(3):
        v = 0
        for j in xrange(8):
            v |= blocks[i * 8 + j] << j
        byts.append(v)
    '''

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

    #byts.append( (tl << 7) | skip )
    byts.append( (tl << 7) | len_nibs )

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
    prev = get_frame(start)
    for i in xrange(start + JUMP, frames, JUMP):

        options = []
        for o in xrange(-1, 3, 1):
            im = get_frame(i + o)
            b, c = tween(prev, im)
            d = encode(b, c)
            options.append((d, im))
        options.sort(key=lambda x: len(x[0]))
        d, im = options[0]

        print ', '.join(map(hex, d)) + ','

        count += 1
        total += len(d)
        prev = im

    print """ };"""
    print "#define FRAMES_BYTES", total
    print "#define FRAMES_W", WANT_WIDTH
    print "#define FRAMES_H", WANT_HEIGHT
    print "#define FRAMES_BS", BS
    print "#define FRAMES_BW", BW
    print "#endif"

process(1, 6565)
#process(1, 1000)
#process(475, 482)

'''
prev = get_frame(1)
im = get_frame(1182)
#im.show()
b, c = tween(prev, im)
#print b, c
d = encode(b, c)
'''
