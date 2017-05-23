# tetris
tetris based on Linux command line

<h1>V1.0</h1> 
实现基本的左右移动、加速、旋转及暂停功能。<br>
说明：<br>
1. 数据结构：使用bool型二维数组存储指定位置是否有方块<br>
    每种形状包含四个方块，每个方块记录该方块绝对位置<br>
2. 使用新线程实现键盘监听，且左右移动、加速下移、旋转是在该线程中实现的<br>
3. 键盘监听使用读取/dev/input中的方式<br>
4. 旋转算法：<br>
    (x,y)以(x<sub>0</sub>,y<sub>0</sub>)为旋转中心逆时针旋转90度，新坐标为(x<sub>0</sub>+y<sub>0</sub>-y,y<sub>0</sub>-x<sub>0</sub>+x)<br>
问题：<br>
1. 屏幕偶尔有闪烁现象<br>
2. 屏幕有光标闪烁<br>
3. 按方向键时，屏幕有 ^[A ^[B ^[C ^[D 的回显<br>
