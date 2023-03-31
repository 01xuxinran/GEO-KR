#!/usr/bin/env python
# coding: utf-8

# In[1]:


import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import networkx as nx
import itertools
import re


# In[ ]:





# In[2]:


colors=['red','orange','royalblue','yellow']


# In[3]:


#1层图
nodeData=pd.read_csv(r'/share/home/2021/xxr/daimashizhan/kgdata/mian/node.csv')
edgeData=pd.read_csv(r'/share/home/2021/xxr/daimashizhan/kgdata/mian/edge.csv')


#face nodes=34
#line nodes=344

layers=[
(nx.Graph(),(0,0)), #第1层

]

ranges = [(0, 34)]
for (layer, coord), (start, end) in zip(layers, ranges):
    layer.add_nodes_from(itertools.islice(nodeData['id'], start, end))
    
    #计算每个节点的坐标，并分类储存
all_pos = {}
all_pos1={}


for layer, (sx, sy) in layers:
    if len(layer.nodes)==34: 
        pos = nx.circular_layout(layer, scale=1) 
        for node in pos:
            all_pos1[node] = pos[node]
            all_pos1[node] += (10 * sx, 0.3* sy) 
    for node in pos:
        all_pos[node] = pos[node]
        all_pos[node] += (10 * sx, 0.3* sy)

fig=plt.figure(figsize=(8,8))
G = nx.Graph()

for i in range(nodeData.shape[0]):
    G.add_node(nodeData['id'][i])

p1=nx.draw(G, pos=all_pos1,node_size=150,font_size=4,nodelist=nodeData['id'][0:],node_color=colors[0],with_labels=True,label='face')

plt.legend(title='Node types')

color_map=[]
for node in G.nodes:
    a=re.split(r'\d+',node)
    if a[0]=='face':
        color_map.append(colors[0])
   
for i in range(edgeData.shape[0]):
    G.add_edge(edgeData['source'][i],edgeData['target'][i])

nx.draw(G, pos=all_pos,node_size=150,font_size=15,node_color=color_map,with_labels=False,width=0.8,alpha=0.2,edge_color='black')
plt.savefig(r'/share/home/2021/xxr/daimashizhan/data/face.png')
#plt.close(fig)


# In[4]:


#2层图
nodeData=pd.read_csv(r'/share/home/2021/xxr/daimashizhan/kgdata/mianbian/node.csv')
edgeData=pd.read_csv(r'/share/home/2021/xxr/daimashizhan/kgdata/mianbian/edge.csv')

#face nodes=34
#line nodes=172

layers=[
(nx.Graph(),(0,10)), #第2层
(nx.Graph(),(0,0)), #第1层
(nx.Graph(),(0,0)), #第1层
]

ranges = [(0, 34), (34,100),(100,206)]
for (layer, coord), (start, end) in zip(layers, ranges):
    layer.add_nodes_from(itertools.islice(nodeData['id'], start, end))

#计算每个节点的坐标，并分类储存
all_pos = {}
all_pos1={}
all_pos2={}
all_pos3={}
for layer, (sx, sy) in layers:
    if len(layer.nodes)==34: 
        pos = nx.circular_layout(layer, scale=1) 
        for node in pos:
            all_pos1[node] = pos[node]
            all_pos1[node] += (10 * sx, 0.3* sy) 
    elif len(layer.nodes)==66: 
        pos = nx.circular_layout(layer, scale=2.5)
        for node in pos:
            all_pos2[node] = pos[node]
            all_pos2[node] += (10 * sx, 0.3* sy)   
    elif len(layer.nodes)==106: 
        pos = nx.circular_layout(layer, scale=3)
        for node in pos:
            all_pos3[node] = pos[node]
            all_pos3[node] += (10 * sx, 0.3* sy)   
    for node in pos:
        all_pos[node] = pos[node]
        all_pos[node] += (10 * sx, 0.3* sy)

fig=plt.figure(figsize=(8,8))
G = nx.Graph()


#for i in range(nodeData.shape[0]):
 #   G.add_node(nodeData['id'][i])
    
# 添加节点标签
labels = {}
for i in range(nodeData.shape[0]):
    labels[nodeData['id'][i]] = nodeData['id'][i]
nx.draw_networkx_labels(G, pos=all_pos, labels=labels, font_size=8)

p1=nx.draw(G, pos=all_pos1,node_size=150,font_size=4,nodelist=nodeData['id'][0:34],node_color=colors[0],with_labels=True,label='face')
p2=nx.draw(G, pos=all_pos2,node_size=150,font_size=4,nodelist=nodeData['id'][34:100],node_color=colors[1],with_labels=True,label='line')
p3=nx.draw(G, pos=all_pos3,node_size=150,font_size=4,nodelist=nodeData['id'][100:],node_color=colors[1],with_labels=True)
plt.legend(title='Node types')

color_map=[]
for node in G.nodes:
    a=re.split(r'\d+',node)
    if a[0]=='face':
        color_map.append(colors[0])
    elif a[0]=='line':
        color_map.append(colors[1])
for i in range(edgeData.shape[0]):
    G.add_edge(edgeData['source'][i],edgeData['target'][i])
    
for i in range(nodeData.shape[0]):
    G.add_node(nodeData['id'][i])


nx.draw(G, pos=all_pos,node_size=150,font_size=5,node_color=color_map,with_labels=False,width=0.8,alpha=0.2,edge_color='black')
plt.savefig(r'/share/home/2021/xxr/daimashizhan/data/face1.png')
#plt.close(fig)


# In[33]:


#3层图
nodeData=pd.read_csv(r'/share/home/2021/xxr/daimashizhan/kgdata/mianbiandian/node.csv')
edgeData=pd.read_csv(r'/share/home/2021/xxr/daimashizhan/kgdata/mianbiandian/edge.csv')

#face nodes=34
#line nodes=172
#point nodes=216

layers=[
(nx.Graph(),(0,20)), #第一层
(nx.Graph(),(0,10)), #第一层
(nx.Graph(),(0,10)), #第一层
(nx.Graph(),(0,0)), #第二层
(nx.Graph(),(0,0)), #第二层
]

ranges = [(0, 34), (34,100),(100,206),(206,306),(306,422)]
for (layer, coord), (start, end) in zip(layers, ranges):
    layer.add_nodes_from(itertools.islice(nodeData['id'], start, end))

#计算每个节点的坐标，并分类储存
all_pos = {}
all_pos1={}
all_pos2={}
all_pos3={}
all_pos4={}
all_pos5={}
for layer, (sx, sy) in layers:
    if len(layer.nodes)==34: 
        pos = nx.circular_layout(layer, scale=1) 
        for node in pos:
            all_pos1[node] = pos[node]
            all_pos1[node] += (10 * sx, 0.3* sy) 
    elif len(layer.nodes)==66: 
        pos = nx.circular_layout(layer, scale=1)
        for node in pos:
            all_pos2[node] = pos[node]
            all_pos2[node] += (10 * sx, 0.3* sy)   
    elif len(layer.nodes)==106: 
        pos = nx.circular_layout(layer, scale=1.5)
        for node in pos:
            all_pos3[node] = pos[node]
            all_pos3[node] += (10 * sx, 0.3* sy)   
    elif len(layer.nodes)==100: 
        pos = nx.circular_layout(layer, scale=1.5)
        for node in pos:
            all_pos4[node] = pos[node]
            all_pos4[node] += (10 * sx, 0.3* sy)   
    elif len(layer.nodes)==116: 
        pos = nx.circular_layout(layer, scale=2)
        for node in pos:
            all_pos5[node] = pos[node]
            all_pos5[node] += (10 * sx, 0.3* sy) 
    for node in pos:
        all_pos[node] = pos[node]
        all_pos[node] += (10 * sx, 0.3* sy)

fig=plt.figure(figsize=(8,8))
G = nx.Graph()
    

p1=nx.draw(G, pos=all_pos1,node_size=50,font_size=4,nodelist=nodeData['id'][0:34],node_color=colors[0],with_labels=True,label='face')
p2=nx.draw(G, pos=all_pos2,node_size=50,font_size=4,nodelist=nodeData['id'][34:100],node_color=colors[1],with_labels=True,label='line')
p3=nx.draw(G, pos=all_pos3,node_size=50,font_size=4,nodelist=nodeData['id'][100:206],node_color=colors[1],with_labels=True)
p4=nx.draw(G, pos=all_pos4,node_size=50,font_size=4,nodelist=nodeData['id'][206:306],node_color=colors[2],with_labels=True,label='point')
p5=nx.draw(G, pos=all_pos5,node_size=50,font_size=4,nodelist=nodeData['id'][306:],node_color=colors[2],with_labels=True)
plt.legend(title='Node types')

color_map=[]
for node in G.nodes:
    a=re.split(r'\d+',node)
    if a[0]=='face':
        color_map.append(colors[0])
    elif a[0]=='line':
        color_map.append(colors[1])
    elif a[0]=='point':
        color_map.append(colors[2])

    
for i in range(nodeData.shape[0]):
    G.add_node(nodeData['id'][i])
for i in range(edgeData.shape[0]):
    G.add_edge(edgeData['source'][i],edgeData['target'][i])

nx.draw(G, pos=all_pos,node_size=50,font_size=5,node_color=color_map,with_labels=False,width=0.8,alpha=0.2,edge_color='black')
plt.savefig(r'/share/home/2021/xxr/daimashizhan/data/face+line+point.png')
#plt.close(fig)


# In[32]:


#4层图
nodeData=pd.read_csv(r'/share/home/2021/xxr/daimashizhan/kgdata/kuai/node.csv')
edgeData=pd.read_csv(r'/share/home/2021/xxr/daimashizhan/kgdata/kuai/edge.csv')

#block nodes=17
#face nodes=175
#line nodes=364
#point node=216
colors1=['green','red','orange','royalblue']
layers=[
(nx.Graph(),(0,50)), #第4层
(nx.Graph(),(0,35)), #第3层
(nx.Graph(),(0,35)), #第3层
(nx.Graph(),(0,0)), #第2层
(nx.Graph(),(0,0)), #第2层
(nx.Graph(),(0,0)), #第2层
(nx.Graph(),(0,20)), #第1层
(nx.Graph(),(0,20)), #第1层
]

ranges = [(0, 17), (17,92),(92,192),(192,356),(356,476),(476,556),(556,662),(662,772)]
for (layer, coord), (start, end) in zip(layers, ranges):
    layer.add_nodes_from(itertools.islice(nodeData['id'], start, end))

#计算每个节点的坐标，并分类储存
all_pos = {}
all_pos1={}
all_pos2={}
all_pos3={}
all_pos4={}
all_pos5={}
all_pos6={}
all_pos7={}
all_pos8={}
for layer, (sx, sy) in layers:
    if len(layer.nodes)==17: 
        pos = nx.circular_layout(layer, scale=2.5) 
        for node in pos:
            all_pos1[node] = pos[node]
            all_pos1[node] += (10 * sx, 0.3* sy) 
    elif len(layer.nodes)==75: 
        pos = nx.circular_layout(layer, scale=2.5)
        for node in pos:
            all_pos2[node] = pos[node]
            all_pos2[node] += (10 * sx, 0.3* sy)   
    elif len(layer.nodes)==100: 
        pos = nx.circular_layout(layer, scale=3)
        for node in pos:
            all_pos3[node] = pos[node]
            all_pos3[node] += (10 * sx, 0.3* sy)   
    elif len(layer.nodes)==164: 
        pos = nx.circular_layout(layer, scale=5.5)
        for node in pos:
            all_pos4[node] = pos[node]
            all_pos4[node] += (10 * sx, 0.3* sy)   
    elif len(layer.nodes)==120: 
        pos = nx.circular_layout(layer, scale=4.5)
        for node in pos:
            all_pos5[node] = pos[node]
            all_pos5[node] += (10 * sx, 0.3* sy) 
    elif len(layer.nodes)==80: 
        pos = nx.circular_layout(layer, scale=3.5)
        for node in pos:
            all_pos6[node] = pos[node]
            all_pos6[node] += (10 * sx, 0.3* sy) 
    elif len(layer.nodes)==106: 
        pos = nx.circular_layout(layer, scale=3.5)
        for node in pos:
            all_pos7[node] = pos[node]
            all_pos7[node] += (10 * sx, 0.3* sy) 
    elif len(layer.nodes)==110: 
        pos = nx.circular_layout(layer, scale=3)
        for node in pos:
            all_pos8[node] = pos[node]
            all_pos8[node] += (10 * sx, 0.3* sy) 
    for node in pos:
        all_pos[node] = pos[node]
        all_pos[node] += (10 * sx, 0.3* sy)

fig=plt.figure(figsize=(8,8))
G = nx.Graph()

ranges = [(0, 17), (17,92),(92,192),(192,356),(356,556),(556,662),(662,772)]

p1=nx.draw(G, pos=all_pos1,node_size=50,font_size=4,nodelist=nodeData['id'][0:17],node_color=colors1[0],with_labels=True,label='block')
p2=nx.draw(G, pos=all_pos2,node_size=50,font_size=4,nodelist=nodeData['id'][17:92],node_color=colors1[1],with_labels=True,label='face')
p3=nx.draw(G, pos=all_pos3,node_size=50,font_size=4,nodelist=nodeData['id'][92:192],node_color=colors1[1],with_labels=True)
p7=nx.draw(G, pos=all_pos7,node_size=50,font_size=4,nodelist=nodeData['id'][556:662],node_color=colors1[2],with_labels=True,label='line')
p8=nx.draw(G, pos=all_pos8,node_size=50,font_size=4,nodelist=nodeData['id'][662:],node_color=colors1[2],with_labels=True)
p4=nx.draw(G, pos=all_pos4,node_size=50,font_size=4,nodelist=nodeData['id'][192:356],node_color=colors1[3],with_labels=True,label='point')
p5=nx.draw(G, pos=all_pos5,node_size=50,font_size=4,nodelist=nodeData['id'][356:476],node_color=colors1[3],with_labels=True)
p6=nx.draw(G, pos=all_pos6,node_size=50,font_size=4,nodelist=nodeData['id'][476:556],node_color=colors1[3],with_labels=True)
#plt.legend(title='Node types',loc='lower center',ncol=4)
plt.legend(title='Node types')

color_map=[]
for node in G.nodes:
    a=re.split(r'\d+',node)
    if a[0]=='block':
        color_map.append(colors1[0])
    elif a[0]=='face':
        color_map.append(colors1[1])
    elif a[0]=='line':
        color_map.append(colors1[3])
    elif a[0]=='point':
        color_map.append(colors1[2])
G = nx.Graph()
    
for i in range(nodeData.shape[0]):
    G.add_node(nodeData['id'][i])
    
for i in range(edgeData.shape[0]):
    G.add_edge(edgeData['source'][i],edgeData['target'][i])

nx.draw(G, pos=all_pos,node_size=20,font_size=2,node_color=color_map,with_labels=False,width=0.4,alpha=0.2,edge_color='black')
plt.savefig(r'/share/home/2021/xxr/daimashizhan/data/kuai1.png')
# plt.close(fig)


# In[1]:


pip install opencv-python


# In[ ]:




