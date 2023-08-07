########################处理数据，可视化模型(以填充方式可视化)
import version5.Struct_5 as st5
import csv
from mpl_toolkits.mplot3d import axes3d
import numpy as np
import matplotlib.pyplot as plt
import version5.Bounds_up_down_layOut as bl
import version5.Fault_layOut as fl
import version5.Layer_layerOut as ll
import version5.Location as loc

# import version6.test as test

# 打开画图窗口1，在三维空间中绘图
from version5.toFigureData import processData

fig = plt.figure(1)
ax = fig.gca(projection='3d')


#########确定块面线点个数
def main():
    ##********************************读取数据********读取点线面块实体，属性，关系数据并存储，以方便后续的处理操作**********************###
    # ---------------------------------读实体数据（点线面块个数），首先存储逻辑实体----------------------------------------#
    blocks_size = 0
    surfaces_size = 0
    lines_size = 0
    points_size = 0
    readType = csv.reader(open("E:/PythonFile/topologyModeling/version5/error_topology/entities_count.csv"))

    # readType = csv.reader(open("E:/PythonFile/topologyModeling/version5/entities_count.csv"))
    for row in readType:
        if len(row) == 0:  # 去除末尾行
            continue
        if row[0] == 'b':
            blocks_size = int(row[1])
        elif row[0] == 's':
            surfaces_size = int(row[1])
        elif row[0] == 'l':
            lines_size = int(row[1])
        elif row[0] == 'p':
            points_size = int(row[1])
    # 存储逻辑块的信息
    blocks = [st5.block(-1)] * blocks_size
    # 存储逻辑面的信息
    surfaces = [st5.surface(-1)] * surfaces_size
    # 存储逻辑线的信息
    lines = [st5.line(-1)] * lines_size
    # 存储逻辑点的信息
    points = [st5.point(-1, -1.0, -1.0, -1.0)] * points_size
    # ------------------从原始数据直接提取拓扑关系，确定块面关系，面线关系，线点关系，块块关系，面面关系，线线关系----------------------#
    draw_topology_relation(blocks, surfaces, lines, points)
    ###断层
    faults = draw_propertise(surfaces)

    ###顶面，底面，边界面布局
    # up_down_surfaces = [st5.up_down_surface(-1)] * 2  ##得到上下顶面
    bounds = [st5.bound(-1)] * 4
    up_down_surfaces = []
    up_down_surfaces_ids = []
    for surface in surfaces:
        if surface.type == 0:
            up_down_surfaces_ids.append(surface.s_id)

    for num in range(0, 2):
        surface = st5.up_down_surface(num)
        surface.s_id = up_down_surfaces_ids[num]
        surfaces[up_down_surfaces_ids[num]].order = num
        up_down_surfaces.append(surface)

    bl.bounds_up_down_location(surfaces, lines, points, up_down_surfaces, bounds)
    #
    # # -------------------------将初始框架进行可视化------------------------------#
    for surface in up_down_surfaces:
        s_id = surface.s_id
        for line_id in surfaces[s_id].lines:
            p1 = points[lines[line_id].points[0]]
            p2 = points[lines[line_id].points[1]]
            figure = ax.plot([p1.x, p2.x], [p1.y, p2.y], [p1.z, p2.z], 'gray')
    for num in range(0, 4):
        p1 = points[up_down_surfaces[0].points[num]]
        p2 = points[up_down_surfaces[1].points[num]]
        figure = ax.plot([p1.x, p2.x], [p1.y, p2.y], [p1.z, p2.z], 'gray')
    # plt.show()

    ##层位
    ###有边界线上的点数得到层位个数
    bound_lines = bounds[0].lines
    left_line_id = bound_lines[2]
    ##
    point_num = len(lines[left_line_id].points)

    layerCount = point_num - 2
    layers = [st5.layer(-1)] * layerCount
    # for num in range(0, layerCount):
    #     layers[num].order = num

    ll.layers_location(blocks, surfaces, lines, points, layers, up_down_surfaces, bounds)

    # # # -------------------------将初始层位面进行可视化------------------------------#
    bounds_lines = []  ##存储边界线
    for bound in bounds:
        subject_lines = bound.lines
        if subject_lines[2] not in bounds_lines:
            bounds_lines.append(subject_lines[2])
        if subject_lines[3] not in bounds_lines:
            bounds_lines.append(subject_lines[3])
    bounds_layer_points = [[], [], [], []]
    for num in range(0, 4):
        line_id = bounds_lines[num]
        for p_id in lines[line_id].points:
            bounds_layer_points[num].append(p_id)
    for num in range(1, len(bounds_layer_points[0]) - 1):
        p1 = points[bounds_layer_points[0][num]]
        p2 = points[bounds_layer_points[1][num]]
        p3 = points[bounds_layer_points[2][num]]
        p4 = points[bounds_layer_points[3][num]]
        figure1 = ax.plot([p1.x, p2.x], [p1.y, p2.y], [p1.z, p2.z], 'green')
        figure2 = ax.plot([p1.x, p3.x], [p1.y, p3.y], [p1.z, p3.z], 'green')
        figure3 = ax.plot([p2.x, p4.x], [p2.y, p4.y], [p2.z, p4.z], 'green')
        figure4 = ax.plot([p4.x, p3.x], [p4.y, p3.y], [p4.z, p3.z], 'green')
    # plt.show()

    fl.fault_LayOut(blocks, surfaces, lines, points, bounds, layers, faults)

    loc.get_location(blocks, surfaces, lines, points, bounds, layers, faults)
    # # -------------------------将断层面框架进行可视化------------------------------#
    for fault in faults:
        # beyound_layer_lines=[]
        ##将断层不在层位面上的线存储
        for line_id in surfaces[fault.f_id].lines:
            flag = False
            for s_id in lines[line_id].surfaces:
                if surfaces[s_id].type == 2:
                    flag = True
                    break
            if flag:
                continue
            for i in range(0, len(lines[line_id].points) - 1):
                p1 = points[lines[line_id].points[i]]
                p2 = points[lines[line_id].points[i + 1]]
                figure = ax.plot([p1.x, p2.x], [p1.y, p2.y], [p1.z, p2.z], 'b')

        first_p = fault.order_points[0]
        last_p = fault.order_points[len(fault.order_points) - 1]
        first_order = fault.relate_layer_order[0]
        last_order = fault.relate_layer_order[len(fault.relate_layer_order) - 1]

        first_line_id =-1
        for line_id in surfaces[fault.f_id].lines:
            if first_line_id!=-1:
                break
            if first_p in lines[line_id].points:
                for s_id in lines[line_id].surfaces:
                    if surfaces[s_id].type==2:
                        first_line_id=line_id
                        break
        last_line_id = -1
        for line_id in surfaces[fault.f_id].lines:
            if last_line_id != -1:
                break
            if last_p in lines[line_id].points:
                for s_id in lines[line_id].surfaces:
                    if surfaces[s_id].type == 2:
                        last_line_id = line_id
                        break
        for i in range(0, len(lines[first_line_id].points) - 1):
            p1 = points[lines[first_line_id].points[i]]
            p2 = points[lines[first_line_id].points[i + 1]]
            figure = ax.plot([p1.x, p2.x], [p1.y, p2.y], [p1.z, p2.z], 'b')
        for i in range(0, len(lines[last_line_id].points) - 1):
            p1 = points[lines[last_line_id].points[i]]
            p2 = points[lines[last_line_id].points[i + 1]]
            figure = ax.plot([p1.x, p2.x], [p1.y, p2.y], [p1.z, p2.z], 'b')
    # plt.show()

    ###TODO 将面渲染颜色
    # ##按照线所在面的不同将线以不同颜色展现
    # -------------------------进行可视化------------------------------#
    for surface in surfaces:

        if surface.type == 0 or surface.type == 1:
            for line_id in surface.lines:
                flag = False
                for s_id in lines[line_id].surfaces:
                    if surfaces[s_id].type == 2 or surfaces[s_id].type == 3:
                        flag = True
                        break
                if flag:
                    continue
                for i in range(0, len(lines[line_id].points) - 1):
                    p1 = points[lines[line_id].points[i]]
                    p2 = points[lines[line_id].points[i + 1]]
                    # x,y = np.meshgrid([p1.x, p2.x],[p1.y, p2.y])
                    figure = ax.plot([p1.x, p2.x], [p1.y, p2.y], [p1.z, p2.z], 'gray')
        if surface.type == 2:
            for line_id in surface.lines:
                for i in range(0, len(lines[line_id].points) - 1):
                    p1 = points[lines[line_id].points[i]]
                    p2 = points[lines[line_id].points[i + 1]]
                    if surface.order == 0:
                        figure = ax.plot([p1.x, p2.x], [p1.y, p2.y], [p1.z, p2.z], 'blue')
                    elif surface.order == 1:
                        figure = ax.plot([p1.x, p2.x], [p1.y, p2.y], [p1.z, p2.z], 'goldenrod')
                    elif surface.order == 2:
                        figure = ax.plot([p1.x, p2.x], [p1.y, p2.y], [p1.z, p2.z], 'darkkhaki')
                    else:
                        figure = ax.plot([p1.x, p2.x], [p1.y, p2.y], [p1.z, p2.z], 'darkviolet')

        if surface.type == 3:
            for line_id in surface.lines:
                for i in range(0, len(lines[line_id].points) - 1):
                    p1 = points[lines[line_id].points[i]]
                    p2 = points[lines[line_id].points[i + 1]]
                    figure = ax.plot([p1.x, p2.x], [p1.y, p2.y], [p1.z, p2.z], 'green')
    plt.show()
    processData(blocks, surfaces, lines, points)


# ------------------拓扑关系，确定块面关系，面线关系，线点关系，块块关系，面面关系，线线关系----------------------#
# ------------------------------------存储逻辑实体之间的关系----------------------------------------#
def draw_topology_relation(blocks, surfaces, lines, points):
    block_id_visited = []  ####存储已经访问过的block_id
    surface_id_visited = []  ####存储已经访问过的surface_id
    line_id_visited = []  ####存储已经访问过的line_id
    point_id_visited = []  ####存储已经访问过的point_id
    # readTopo = csv.reader(open("E:/PythonFile/topologyModeling/version5/topology_relationship.csv"))
    readTopo = csv.reader(open("E:/PythonFile/topologyModeling/version5/error_topology/topology_relationship.csv"))
    count = 0
    for row in readTopo:
        if count == 0:
            count += 1
            continue
        if len(row) == 0:  # 去除末尾行
            continue
        if row[2] == 'Undirected':  # 表示相同逻辑实体，存储同级逻辑实体间的关系。解决同级的关联关系
            brother_0 = row[0]
            brother_1 = row[1]
            brother_0_id = int(brother_0[1:]) - 1
            brother_1_id = int(brother_1[1:]) - 1
            type = brother_0[0]  #######表示属性，是块，面，线还是点
            ###########把面和面邻接关系先找出来
            if type == 'b':
                if brother_0_id not in block_id_visited:
                    blocks[brother_0_id] = st5.block(brother_0_id)
                    blocks[brother_0_id].related_b.append(brother_1_id)
                    block_id_visited.append(brother_0_id)
                else:
                    blocks[brother_0_id].related_b.append(brother_1_id)
                if brother_1_id not in block_id_visited:
                    blocks[brother_1_id] = st5.block(brother_1_id)
                    blocks[brother_1_id].related_b.append(brother_0_id)
                    block_id_visited.append(brother_1_id)
                else:
                    blocks[brother_1_id].related_b.append(brother_0_id)
            elif type == 's':
                if brother_0_id not in surface_id_visited:
                    surfaces[brother_0_id] = st5.surface(brother_0_id)
                    surfaces[brother_0_id].related_s.append(brother_1_id)
                    surface_id_visited.append(brother_0_id)
                else:
                    surfaces[brother_0_id].related_s.append(brother_1_id)
                if brother_1_id not in surface_id_visited:
                    surfaces[brother_1_id] = st5.surface(brother_1_id)
                    surfaces[brother_1_id].related_s.append(brother_0_id)
                    surface_id_visited.append(brother_1_id)
                else:
                    surfaces[brother_1_id].related_s.append(brother_0_id)
            elif type == 'l':
                if brother_0_id not in line_id_visited:
                    lines[brother_0_id] = st5.line(brother_0_id)
                    lines[brother_0_id].related_l.append(brother_1_id)
                    line_id_visited.append(brother_0_id)
                else:
                    lines[brother_0_id].related_l.append(brother_1_id)
                if brother_1_id not in line_id_visited:
                    lines[brother_1_id] = st5.line(brother_1_id)
                    lines[brother_1_id].related_l.append(brother_0_id)
                    line_id_visited.append(brother_1_id)
                else:
                    lines[brother_1_id].related_l.append(brother_0_id)
        if row[2] == 'Directed':  # 表示不同级逻辑实体，存储上下级逻辑实体间的关系。 解决上下级的隶属关系
            son = row[0]
            father = row[1]
            son_id = int(son[1:]) - 1
            father_id = int(father[1:]) - 1
            type = son[0]  #######表示属性，是块，面，线还是点
            #####面和块的隶属关系########
            if type == 's':
                surfaces[son_id].blocks.append(father_id)
                blocks[father_id].surfaces.append(son_id)
            #####线和面的隶属关系
            if type == 'l':
                lines[son_id].surfaces.append(father_id)
                surfaces[father_id].lines.append(son_id)
            if type == 'p':
                if son_id not in point_id_visited:
                    points[son_id] = st5.point(son_id)
                    points[son_id].lines.append(father_id)
                    point_id_visited.append(son_id)
                else:
                    points[son_id].lines.append(father_id)
                lines[father_id].points.append(son_id)
    #####找到点隶属的面
    for point in points:
        for line_id in point.lines:
            for surface_id in lines[line_id].surfaces:
                if surface_id not in point.surfaces:
                    point.surfaces.append(surface_id)


##-----------------------------------------确定每个面的属性，写错了应该是--------------------------------------------##
def draw_propertise(surfaces):
    readSurfaceType = csv.reader(open("E:/PythonFile/topologyModeling/version5/error_topology/face_propertise.csv"))
    # readSurfaceType = csv.reader(open("E:/PythonFile/topologyModeling/version5/face_propertise.csv"))
    faults = []
    already_sur_id = []
    for row in readSurfaceType:
        sur = row[0]
        sur_id = int(sur[1:]) - 1
        surfaces[sur_id].type = int(row[1])
        if int(row[1]) == 3 and sur_id not in already_sur_id:  ##防止重复
            fault = st5.fault(sur_id)
            fault.faultType = int(row[2])
            surfaces[sur_id].faultType = int(row[2])  ##断层属性
            faults.append(fault)
            already_sur_id.append(sur_id)
    return faults


##得到断层的相关信息，层位的相关信息
def layers_faults_info(surfaces, layers, faults, layerCount, faultCount):
    for num in range(0, layerCount):
        for surface in surfaces:
            if surface.type == 2 and surface.order == num:
                layers[num].order = num
                layers[num].sublayer.append(surface.s_id)
                layers[num].l_id = surface.s_id
                for related_id in surface.related_s:
                    if surfaces[related_id].type == 3 and related_id not in layers[num].faults:
                        layers[num].faults.append(related_id)

    for num in range(0, faultCount):
        for surface in surfaces:
            if surface.type == 3:
                faults[num].f_id = surface.s_id
                faults[num].faultType = surface.faultType
                for related_id in surface.related_s:
                    if surfaces[related_id].type == 2 and related_id not in faults[num].layers:
                        faults[num].layers.append(related_id)
                    elif surfaces[related_id].type == 1 and related_id not in faults[num].surfaces_bound:
                        faults[num].surfaces_bound.append(related_id)


###获得顶面底面的信息
def get_up_down_surfaces(surfaces, lines, up_down_surfaces):
    for num in range(0, 2):
        for surface in surfaces:
            if surface.type == 0:
                up_down_surfaces[num].s_id = surface.s_id


if __name__ == '__main__':
    main()
