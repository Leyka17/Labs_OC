import random
f = open('text.txt', 'w')
result=""
result+='a'*4092+'hello'+'aaa\n'+'a'*5000

pos=0
size=len(result)
print(size)
position=[]
loop=True
while loop:
    new_pos=result.find('hello',pos)
    #print(new_pos)
    if new_pos==-1:
        break
    position.append(new_pos)
    pos=new_pos+1
print(result)
f.write(result)
print(position)
