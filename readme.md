# グローバル戦術とローカル戦術の通信プログラム

# 実行方法
## 依存しているプログラム
`git` `cmake` `gcc-8` `g++-8` `make`
## 
適切なディレクトリーにて`git clone`する 
```
cd glcom
mkdir build -p
cd build
cmake ..
make -j
./glcom
```
#通信内容
バイナリ形式で
```
(Frame)::= (Direction) (Ball) (RobotID Position) n:uint8 (RobotID Position)^n (Command)
(Direction) ::= RightToLeft| LeftToRight 
(Target)::= (NamedPosition) | (Position) | (RobotID)
(NamedPosition) ::= Center|GoalRight|GoalLeft
(Position)::= X:float Y:float Theta:float
(RobotID)::=(Team) number:uint8
(Team) ::=Blue| Yellow
(Ball) ::=x:float y:float

(Command)::=(Move)|(Send)|(Receive)|(Block)|..  
(Move)::=(Target)
(Send)::=(Target)
(Block)::=(Target)

```
補足: `()`は変数、`a|b`は