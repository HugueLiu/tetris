# tetris
tetris based on Linux command line

<h1>V1.0</h1> 
实现基本的左右移动、加速、旋转及暂停功能。
说明：
1. 数据结构：使用bool型二维数组存储指定位置是否有方块
    每种形状包含四个方块，每个方块记录该方块绝对位置 
2. 使用新线程实现键盘监听，且左右移动、加速下移、旋转是在该线程中实现的。
3. 键盘监听使用 /dev/input中的监听方式
4. 旋转算法：
    (x,y)以(x<sub>0</sub>,y<sub>0</sub>)为旋转中心逆时针旋转90度，新坐标为(x<sub>0</sub>+y<sub>0</sub>-y,y<sub>0</sub>-x<sub>0</sub>+x)。
