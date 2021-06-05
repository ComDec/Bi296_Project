import random
import math
import numpy as np
import sys

def geneseq():
    store=""
   
    for i in range(0,1000):
        if(random.uniform(0,100)<29.5):
            store+='A'
        elif(random.uniform(0,100)<59):
            store+='T'
        elif(random.uniform(0,100)<79.5):
            store+='C'
        else:
            store+='G'
    return store
def code(gene):
    p=[0.1,0.2,0.3,0.4]
    ch='ATCG'
    pzone=[0.0]
    for i in range(1,4):
        pzone.append(float(pzone[i-1])+p[i-1])
    strlen=int(20)
    #target=gene[0:strlen]
    filename = str(sys.argv[1])
    tar=open(filename,mode='r')
    trans=tar.read()
    length=len(trans)
    #target=np.fromstring(target,dtype=np.uint8)
    #target=target.tostring
    print(trans)
    count=0
    while((length/20)>count):
        #target=trans[int((count)*20),int((count+1)*20)]
        s=slice(count*20,(count+1)*20)
        target=list(trans[s])
        #print("*******compress*******")
        low=0.0
        z=1.0
        for i in range(0,strlen):
            for j in range(0,4):
                if(target[i]==ch[j]):
                    cp=float(p[j])
                    L=float(pzone[j])
                    low=low+float(z)*pzone[j]
                    z=float(z)*p[j]
                    break
        result=float(low)+z
        binary=[]
        infolen=float(math.log(1/z))/float(math.log(2))
        if(infolen>int(infolen)):
            infolen=int(infolen)+1
        else:
            infolen=int(infolen)
        for i in range(0,infolen):
            result=result*2
            if(result>1):
                result=result-1
                binary.append(1)
            elif(result<1):
                binary.append(0)
            else:
                break
        if(i>=infolen):
            for j in range(i,0,-1):
                binary[j-1]=binary[j-1]+1%2
                if(binary[j-1]==1):
                    break
        print("\n*******The binary compress result*******")
        for j in range(0,i):
            print(binary[j],end='')
        print("\n*******uncompress*******")
        w=0.5
        newlow=0.0
        newz=1.0
        deResult=0.0
        for i in range(0,j+1):
            deResult=deResult+float(w)*binary[i]
            w=w*0.5
        for i in range(0,strlen):
            for j in range(4,0,-1):
                Low=newlow
                Z=newz
                Low=Low+float(Z)*pzone[j-1]
                Z=float(Z)*p[j-1]
                if(deResult>=Low):
                    newlow=Low
                    newz=Z
                    print(ch[j-1],end='')
                    break 
        count=count+1
def main():
    gene=geneseq()
    code(gene)
main()  
