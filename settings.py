# Height of the output video.
WANT_HEIGHT = 64
# Width of the output video. Make sure this is no wider than the original
# aspect ratio.
WANT_WIDTH = 68

# Trim the video
FIRST_FRAME = 40
LAST_FRAME = 2200
#LAST_FRAME = 6523

# Block size. Each block is checked against the last frame for change.
# Decoder is slow if not a multiple of 4.
BLOCK_SIZE = 8

# The number of frames in the orignal video to collapse into one output frame.
JUMP = 6

# Threshold in the grayscale image, below which is considered black. Python 3
# Pillow converts grayscale different from python 2 for some reason.
THRESH = 20

# If positive, blur the image.
BLUR = 0

# If positive, dilate the image.
MAX_FILTER = 5

# If positive, erode the image.
MIN_FILTER = 0

# If frame data is longer than this, delete short runs of pixels
SIMPLIFY_LIMIT = 99999

# When simplifying, how long of a run to delete.
SIMPLIFY_RUN_LENGTH = 2

# If a block only changes one corner, odds of treating that block
# as unchanged
UNCHANGED_CORNER_RANDOM = 50

# How many pixels on the of a block that's change to consider the
# entire block unchanged, when 2 edges have changed.
UNCHANGED_EDGE_LIMIT = 6

# When changes are only on the edges of a block, how many changes
# in the center to also allow. When 2 edges have changed.
UNCHANGED_EDGE_CENTER_LIMIT = 8

# When only one pixel changed on the edge, how many pixels are allowed
# to change in the center?
UNCHANGED_SINGLE_CENTER_LIMIT = 10
