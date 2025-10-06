[README.txt](https://github.com/user-attachments/files/22724763/README.txt)
To run the program there should be a directory inside the project directory structure like:

coin_counter/
|---dataset
|---|images
|---|---|1_CENT
|---|---|1_EUR
|---|---|2_CENT
|---|---|2_EUR
|---|---|10_CENT
|---|---|20_CENT
|---|---|50_CENT
|---|labels
|---|---|1_CENT
...
|---test
|---|images
|---|labels
|---|videos
|---|---|video1_frame
|---|---|video2_frame

From command line, it needs the path of the test image or test video to be evaluated, i.e. ./Euro_Counter coin_counter/test/images/IMG_25.jpg or ./Euro_Counter coin_counter/test/videos/video1.MOV
