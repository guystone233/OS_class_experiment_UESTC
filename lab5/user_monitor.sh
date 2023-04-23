#!/bin/bash
#编写一个用户监测程序usr_monitor, 其运行格式为：usr_monitor username
#其中username是用户指定的任意一个用户名。程序运行时首先列出当前系统中的已登录用户的名单，
#再检查指定用户是否已登录。如果已登录，则显示相应信息；如果未登录，则等待该用户登录，直到指定用户登录进入系统为止。
# （1）根据变量$#的值检查命令行上是否有一个用户名，如果有则继续运行；如果没有或多于一个用户名则输出提示信息：Usage: usr_monitor username
# （2）运行who命令，并将运行结果中的用户名字段截取下来，保存到一个用户变量中。
# （3）echo命令显示该变量的值，即当前系统中的用户名单。
# （4）判断用户名单中是否包含命令行上指定的用户，如果有则输出提示信息：user [username] is logon，并退出程序。
# （5）如果用户名单中没有包含命令行上指定的用户，则输出提示信息：waiting user [username] ...，光标停留在省略号后面。
# （6）使用while循环，用sleep命令每隔5秒钟检查指定用户是否已登录系统。检测方法与步骤2相同。
# （7）如果指定用户未登录，则一直等待下去；如果指定用户已登录，则在省略号后面接着输出提示信息： [username] is log on，退出循环，并结束程序。
if [ $# -ne 1 ]; then
    echo "Usage: usr_monitor username"
    exit 1
fi
username=$1
while true; do
    users=(`who | cut -d " " -f 1`) # 使用who列出当前登录的用户并截取用户名存入数组（括号是为了转化为数组）
    echo "Current user list:${users[@]}"
    for user in ${users[@]}; do
        if [ $user == $username ]; then
            echo "user ${username} is logon"
            exit 0
        fi
    done
    echo "waiting user ${username} ..."
    sleep 5
done