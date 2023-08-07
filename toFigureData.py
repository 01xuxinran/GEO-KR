import version5.Struct_5 as st5
"""
 为了在matlab中成图，需要将数据调整成matlab需要的格式
 使用patch函数
 这里点按照其id顺序存储在points_loc
 最后根据块的顺序把一个个面上的点按顺序读入在
 不能这么搞
"""
def processData(blocks ,surfaces ,lines ,points):
    points_loc =[  ]##存储所有点
    blocks_sur={  }##block_id 作为索引，该块的面id作为值
    sur_p={} ##surface_id 作为索引，该面的点id作为值（按线的邻接顺序排列）

    "存储所有的点"
    while len(points_loc)<len(points):
        cur_index=len(points_loc)
        for p in points:
            if p.p_id==cur_index:
                cur_p_loc=[p.x,p.y,p.z]
                points_loc.append(cur_p_loc)
                break
    with open("points_loc.txt", 'w') as f:
        for cur_p_loc in points_loc:
            for i in cur_p_loc:
                f.write(str(i)+" ")
            f.write('\n')
        f.close()

    # "遍历面，将每个面上的点按顺序id存储"
    # for surface in surfaces:
    #     current_p=[] ##存储当前面的点



