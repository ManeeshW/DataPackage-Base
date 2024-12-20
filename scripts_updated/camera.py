import numpy as np
import cv2
import threading
import os
from datetime import datetime
from common_types import Cam1Data, rover


class LogitecCamera(threading.Thread):
    def __init__(self, thread_id, t0, enabled, freq=5.0):
        threading.Thread.__init__(self)
        self.thread_id = thread_id
        self._lock = threading.Lock()

        self._on = True
        self._enabled = enabled
        self._t0 = t0
        self._t = (datetime.now() - t0).total_seconds()
        self._folder_name = datetime.now().strftime('capture_%Y%m%d_%H%M%S')
        self._freq = freq
        self._desired_dt = 1.0 / freq
        self._logON = 0
        self._idx = 0
        
        self.video = cv2.VideoCapture()

        self.video.open(0, apiPreference=cv2.CAP_V4L2)
        #self.video.set(cv2.CAP_PROP_FRAME_WIDTH, 640)
        #self.video.set(cv2.CAP_PROP_FRAME_HEIGHT, 480)


        self.video.set(cv2.CAP_PROP_FOURCC, cv2.VideoWriter_fourcc('M', 'J', 'P', 'G'))
        
        #self.video.set(cv2.CAP_PROP_AUTO_EXPOSURE, 0)
        #self.video.set(cv2.CAP_PROP_AUTO_WB, 1)
        #self.video.set(cv2.CAP_PROP_FPS, 24)

        print('Camera 01: initialized')

    def run(self):
        if not self._enabled:
            print('Camera has been disabled from settings'
                '\n\tExiting thread')
            return

        freq = self._freq
        t = datetime.now()
        t_pre = datetime.now()
        avg_number = 100.0

        

        #creating camera capture directory
        parent_dir = "/home/pi/onr-data-package/scripts_updated/cam_data"
        directory = self._folder_name
        path = os.path.join(parent_dir, directory)
        os.mkdir(path)
        
        
        idx = 0
        try:
            while self._on:
                
                t = datetime.now()
                dt = (t - t_pre).total_seconds()
                if dt < self._desired_dt:
                    continue

                freq = (freq * (avg_number - 1) + (1.0 / dt)) / avg_number
                t_pre = t

                idx += 1
                img_name = "/{:d}.png".format(idx)
                
                success, color_image = self.video.read()
                # Write images
                cv2.imwrite(path+img_name,color_image)

                # Show images
                #cv2.imwrite(path+img_name,color_image)
                #cv2.namedWindow('RealSense', cv2.WINDOW_AUTOSIZE)
                #cv2.imshow('RealSense', color_image)
                #cv2.waitKey(1)
                with self._lock:
                    self._t = (t - self._t0).total_seconds()
                    self._freq = int(freq)
                    self._idx = idx
                    self.update_data()
                    #print('img ', self._idx , " | freq : ",int(self._freq),'Hz   ',img_name )

        finally:

            # Stop streaming
            print('Camera: Stop streaming')

        print('Camera 1: thread closed')

    def update_data(self):
        data = Cam1Data(
            self._t,
            self._freq,
            self._idx,
        )
        rover.update_cam1(data)

    def end_thread(self):
        self._on = False
        print('Camera: thread close')
        
        
# class RealSenseCamera(threading.Thread):
#     def __init__(self, thread_id, t0, enabled, freq=5.0):
#         threading.Thread.__init__(self)
#         self.thread_id = thread_id
#         self._lock = threading.Lock()
# 
#         self._on = True
#         self._enabled = enabled
#         self._t0 = t0
#         self._t = (datetime.now() - t0).total_seconds()
#         self._folder_name = datetime.now().strftime('capture_%Y%m%d_%H%M%S')
#         self._freq = freq
#         self._desired_dt = 1.0 / freq
#         self._logON = 0
#         self._idx = 0
# 
# 
#         print('Camera 01: initialized')
# 
#     def run(self):
#         if not self._enabled:
#             print('Camera has been disabled from settings'
#                 '\n\tExiting thread')
#             return
# 
#         freq = self._freq
#         t = datetime.now()
#         t_pre = datetime.now()
#         avg_number = 100.0
# 
#         # Configure depth and color streams
#         pipeline = rs.pipeline()
#         config = rs.config()
# 
#         # Get device product line for setting a supporting resolution
#         pipeline_wrapper = rs.pipeline_wrapper(pipeline)
#         pipeline_profile = config.resolve(pipeline_wrapper)
#         device = pipeline_profile.get_device()
#         device_product_line = str(device.get_info(rs.camera_info.product_line))
# 
#         found_rgb = False
#         for s in device.sensors:
#             if s.get_info(rs.camera_info.name) == 'RGB Camera':
#                 found_rgb = True
#                 break
#         if not found_rgb:
#             print("Camera: Depth camera with Color sensor required")
#             exit(0)
# 
#         config.enable_stream(rs.stream.color, 640, 480, rs.format.bgr8, 30)
# 
#         #creating camera capture directory
#         parent_dir = "/home/fdcl/onr-data-package/scripts/cam_data/"
#         directory = self._folder_name
#         path = os.path.join(parent_dir, directory)
#         os.mkdir(path)
#         
#         # Start streaming
#         pipeline.start(config)
#         idx = 0
#         try:
#             while self._on:
#                 
#                 t = datetime.now()
#                 dt = (t - t_pre).total_seconds()
#                 if dt < self._desired_dt:
#                     continue
# 
#                 freq = (freq * (avg_number - 1) + (1.0 / dt)) / avg_number
#                 t_pre = t
# 
#                 # Wait for a coherent pair of frames: depth and color
#                 frames = pipeline.wait_for_frames()
#                 color_frame = frames.get_color_frame()
# 
#                 # Convert images to numpy arrays
#                 color_image = np.asanyarray(color_frame.get_data())
#                 idx += 1
#                 img_name = "/{:d}.png".format(idx)
#                 print('img ', idx, " | freq : ",int(freq),'Hz  ', img_name)
#                 # Write images
#                 cv2.imwrite(path+img_name,color_image)
# 
#                 # Show images
#                 #cv2.imwrite(path+img_name,color_image)
#                 #cv2.namedWindow('RealSense', cv2.WINDOW_AUTOSIZE)
#                 #cv2.imshow('RealSense', color_image)
#                 #cv2.waitKey(1)
#                 with self._lock:
#                     self._t = (t - self._t0).total_seconds()
#                     self._freq = int(freq)
#                     self._idx = idx
#                     self.update_data()
#                     print('img ', self._idx , " | freq : ",int(self._freq),'Hz   ', )
# 
#         finally:
# 
#             # Stop streaming
#             pipeline.stop()
#             print('Camera: Stop streaming')
# 
#         print('Camera 1: thread closed')
# 
#     def update_data(self):
#         data = Cam1Data(
#             self._t,
#             self._freq,
#             self._idx,
#         )
#         rover.update_cam1(data)
# 
#     def end_thread(self):
#         self._on = False
#         print('Camera: thread close')