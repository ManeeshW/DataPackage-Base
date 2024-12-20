import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import pymap3d as pm
from ezplot.ezplot import plot
from data import Data
from scipy import stats
import os
import shutil

plt.style.use('seaborn')

import warnings
warnings.filterwarnings("ignore")

def run():
    with open('recentLogFile.txt') as f:
        logname = f.readlines()
    
    yes = input("Do you want to save log file in specific folder (y/n) ? : ")

    
    if yes == "y" or yes == "Y":
        Dir = "/Users/uav/Documents/onr-data-package/scripts/" + logname[0] + ".txt"


        test_file_name = logname[0][10:]
        test_folder_name = 'Test_'+logname[0][14:18]+'.'+logname[0][18:20]+'.'+logname[0][20:22]

        Dir = '/Users/uav/Documents/onr-data-package/collected/'
        path = os.path.join(Dir, test_folder_name)

        try :   
            os.mkdir(path)
        except FileExistsError:
            pass
        
        test_no = str(len(os.listdir(path + '/')))
        fpath = os.path.join(path+"/", "Test_"+test_no)
        try :   
            os.mkdir(fpath)
        except FileExistsError:
            pass

        newDir = fpath + '/' +  "Test_" + test_no + '_py_' + test_file_name

        shutil.copy2('/Users/uav/Documents/onr-data-package/scripts/data_logs/'+ test_file_name + '_0.txt', newDir + '.txt')


         # #file_name =  newDir+'.csv'
        file_name =   newDir+'.txt'
        # #df = pd.read_csv(file_name)
        df = pd.read_csv(file_name, header=0, skipinitialspace=True)
        cols = ['ypr_0', 'ypr_1', 'ypr_2', 'a_0', 'a_1', 'a_2', 'W_0', 'W_1', 'W_2']

        # Outlier rejection
        Q1 = df[cols].quantile(0.05)
        Q3 = df[cols].quantile(0.95)
        IQR = Q3 - Q1

        df = df[~((df[cols] < (Q1 - 1.5 * IQR)) |(df[cols] > (Q3 + 1.5 * IQR))).any(axis=1)]
        df.to_csv(newDir+'.csv',index_label=None,index=False)
        df.to_excel(newDir+'.xlsx',index_label=None,index=False)
        print("Data files are saved to -> ",fpath)
    else: 
        print('Data files are not saved')