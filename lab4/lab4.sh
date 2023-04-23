#!/bin/bash
#编写一个SHELL程序，动态检测指定文件的状态信息，当文件的大小发生改变时，给出提示信息，并继续前进检测。当文件的大小的变化次数或持续检查无变化次数达到一定值时，退出检查，程序结束。
# （1）清屏；
# （2）提示用户输入要检测其状态的文件名；
# （3）显示该文件的状态信息（提示：该状态信息可由命令ls –l 来得到），或找不到该文件时的错误提示；
# （4）用cut命令，或用sed或awk命令来截取状态信息中文件的大小并保存；
# （5）每隔5秒钟检测一次该文件大小的信息，并与保存的文件原来的大小相比较；
# （6）如果文件大小未改变，则屏幕显示不变，并继续每隔5秒钟检测一次；
# （7）如果文件大小已改变，则保存新的文件大小，并在屏幕上显示：file [ filename ] size changed（括号中的filename为本程序运行时用户输入的被检测的文件名）。程序继续每隔5秒钟检测一次文件的大小；
# （8）程序循环执行5~7步的操作。当被检测的文件或者已累计改变了两次大小，或者已连续被检测了十次还未改变大小时，给出相应提示，然后清屏退出。
clear
echo "Please input the filename to be detected:"
read filename
if [ ! -e $filename ]; then
    echo "File ${filename} not found!"
    exit 1
fi
echo "Current status of ${filename}:"
ls -l $filename
filesize=`ls -l $filename | cut -d " " -f 5`
unchanged=0
changed=0
while true; do
    sleep 5
    if [ $filesize -eq `ls -l $filename | cut -d " " -f 5` ]; then
        unchanged=`expr $unchanged + 1`
        echo "File ${filename} size unchanged."
        if [ $unchanged -eq 10 ]; then
            echo "The file has not been changed for 10 times."
            break
        fi
    else
        filesize=`ls -l $filename | cut -d " " -f 5`
        echo "File ${filename} size changed."
        changed=`expr $changed + 1`
        if [ $changed -eq 2 ]; then
            echo "The file has been changed for 2 times."
            break
        fi
    fi
done