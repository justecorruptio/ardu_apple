Generate the frame data
--

    cd data
    ffmpeg -i source.mp4 'frames/out%06d.png'

Generate frame.h
--

    python2 generate_frames.py > frames.h

Settings
--
    settings in settings.py

