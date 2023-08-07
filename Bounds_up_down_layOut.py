# **************************************顶层、底层、边界面和层位面布局******************************#
import version5.Struct_5 as st5


def bounds_up_down_location(surfaces, lines, points, up_down_surfaces, bounds):
    # -------------------------------先处理顶面，底面，侧面，层位面---------------------------#
    ############0代表顶或底，1代表边界面，2代表层位面，3代表断层面############################

    # ---------------------------------确定顶层和底层------------------------------------#
    ##将bound_fault_num进行大小比较，最大的边界面就是投影面
    # ---------------------------------确定断层的投影面,找到与断层面相交最多的面，将他的order记录------------------------------------#
    project_front_bound_id = -1  ####表示投影面即边界面的id，同时返回该数值
    bound_fault_num = []
    for surface in surfaces:
        if surface.type != 1:
            continue
        else:
            num = 0
            for relate_surface_id in surface.related_s:
                if surfaces[relate_surface_id].type == 3:
                    num += 1
            bound_fault_num.append([surface.s_id, num])

    num = 0
    for i in range(0, len(bound_fault_num)):
        if num < bound_fault_num[i][1]:
            project_front_bound_id = bound_fault_num[i][0]

    already_surfaces = []  # 只存储顶面，层位面和底面
    down_surface_lines = []  # 存储已经遍历过的线
    down_surface_Points = []  # 只存储顶面点，层位面边界点和底面的点

    # 处理底面,选择up_down_surfaces第二个作为底面。从下面开始0，0，0
    down_surface = surfaces[up_down_surfaces[1].s_id]
    # surface.order = 0
    first_line = lines[get_line_id_by_twoSurfaces(surfaces, down_surface.s_id, project_front_bound_id)]
    first_point = points[first_line.points[0]]
    second_point = points[first_line.points[1]]
    first_point.x = 0
    first_point.y = 0
    first_point.z = 0
    second_point.x = 1
    second_point.y = 0
    second_point.z = 0
    ########把点的id存储
    down_surface_Points.append(first_point.p_id)
    down_surface_Points.append(second_point.p_id)
    down_surface_lines.append(first_line.l_id)  # 底面确定的第一条边

    #########找其余点,由前两个点来找后两个点
    for sub_to_lines_id in first_point.lines:
        ###找到与第一个点关联的在顶面的且未被赋完坐标的线id
        if sub_to_lines_id in down_surface.lines and sub_to_lines_id not in down_surface_lines:
            third_point = points[0]
            if lines[sub_to_lines_id].points[0] == first_point.p_id:
                third_point = points[lines[sub_to_lines_id].points[1]]
                third_point.x = first_point.x
                third_point.y = first_point.y + 1
                third_point.z = first_point.z
            elif lines[sub_to_lines_id].points[1] == first_point.p_id:
                third_point = points[lines[sub_to_lines_id].points[0]]
                third_point.x = first_point.x
                third_point.y = first_point.y + 1
                third_point.z = first_point.z
            down_surface_Points.append(third_point.p_id)
            down_surface_lines.append(sub_to_lines_id)
    for sub_to_lines_id in second_point.lines:
        ###找到与第二个点关联的在顶面的且未被赋完坐标的线id
        if sub_to_lines_id in down_surface.lines and sub_to_lines_id not in down_surface_lines:
            forth_point = points[0]
            if lines[sub_to_lines_id].points[0] == second_point.p_id:
                forth_point = points[lines[sub_to_lines_id].points[1]]
                forth_point.x = second_point.x
                forth_point.y = second_point.y + 1
                forth_point.z = second_point.z
            elif lines[sub_to_lines_id].points[1] == second_point.p_id:
                forth_point = points[lines[sub_to_lines_id].points[0]]
                forth_point.x = second_point.x
                forth_point.y = second_point.y + 1
                forth_point.z = second_point.z
            down_surface_Points.append(forth_point.p_id)
            down_surface_lines.append(sub_to_lines_id)

    up_down_surfaces[1].points=down_surface_Points
    ##将底面的第四条边加入down_surface_lines
    for line_id in down_surface.lines:
        if line_id not in down_surface_lines:
            down_surface_lines.append(line_id)
            break

    ##将底面的四条边按顺序存储
    up_down_surfaces[1].ordered_lines = down_surface_lines
    already_surfaces.append(down_surface.s_id)

    ##处理顶面
    up_surface_Points = []
    for line_id in surfaces[up_down_surfaces[0].s_id].lines:
        for p_id in lines[line_id].points:
            if p_id not in up_surface_Points:
                up_surface_Points.append(p_id)

    up_surface_Ordered_points = []
    ##找到与底面对应的顶面上的点，将其赋值坐标
    for p1_id in down_surface_Points:
        for p2_id in up_surface_Points:
            ###找到与底面对应的顶面的点
            if is_inSameLine(points, lines, p1_id, p2_id):
                points[p2_id].x = points[p1_id].x
                points[p2_id].y = points[p1_id].y
                points[p2_id].z = points[p1_id].z + 1
                up_surface_Ordered_points.append(p2_id)
                break
    up_down_surfaces[0].points=up_surface_Ordered_points
    up_ordered_lines = []
    for num in range(0, 4):
        if num == 0:
            p1 = up_surface_Ordered_points[0]
            p2 = up_surface_Ordered_points[1]
            order_line_id = get_line_id_by_twoPoints(lines, p1, p2)
            up_ordered_lines.append(order_line_id)
        elif num == 1:
            p1 = up_surface_Ordered_points[0]
            p2 = up_surface_Ordered_points[2]
            order_line_id = get_line_id_by_twoPoints(lines, p1, p2)
            up_ordered_lines.append(order_line_id)
        elif num == 2:
            p1 = up_surface_Ordered_points[1]
            p2 = up_surface_Ordered_points[3]
            order_line_id = get_line_id_by_twoPoints(lines, p1, p2)
            up_ordered_lines.append(order_line_id)
        elif num == 3:
            p1 = up_surface_Ordered_points[2]
            p2 = up_surface_Ordered_points[3]
            order_line_id = get_line_id_by_twoPoints(lines, p1, p2)
            up_ordered_lines.append(order_line_id)

    up_down_surfaces[0].ordered_lines = up_ordered_lines

    ##按照顶面四条线，把边界面给排个序，并存储在bounds里
    for num in range(0, 4):
        line_id = up_down_surfaces[0].ordered_lines[num]  ##找到边界面的上边线
        face_id = -1  ##边界面id
        for surface_id in lines[line_id].surfaces:
            if surfaces[surface_id].type == 1:
                face_id = surface_id
        bound = st5.bound(num)
        bound.b_id = face_id
        surfaces[face_id].order = num  ## 面的order，保持信息一致
        bound.lines.append(line_id)
        down_line_id = up_down_surfaces[1].ordered_lines[num]
        bound.lines.append(down_line_id)
        left_line_id = -1
        right_line_id = -1
        if num == 0:
            left_line_id = get_line_id_by_twoPoints(lines, down_surface_Points[0], up_surface_Ordered_points[0])
            right_line_id = get_line_id_by_twoPoints(lines, down_surface_Points[1],
                                                     up_surface_Ordered_points[1])
        elif num == 1:
            left_line_id = get_line_id_by_twoPoints(lines, down_surface_Points[2], up_surface_Ordered_points[2])
            right_line_id = get_line_id_by_twoPoints(lines, down_surface_Points[0],
                                                     up_surface_Ordered_points[0])
        elif num == 2:
            left_line_id = get_line_id_by_twoPoints(lines, down_surface_Points[1], up_surface_Ordered_points[1])
            right_line_id = get_line_id_by_twoPoints(lines, down_surface_Points[3],
                                                     up_surface_Ordered_points[3])
        elif num == 3:
            left_line_id = get_line_id_by_twoPoints(lines, down_surface_Points[3], up_surface_Ordered_points[3])
            right_line_id = get_line_id_by_twoPoints(lines, down_surface_Points[2],
                                                     up_surface_Ordered_points[2])
        bound.lines.append(left_line_id)
        bound.lines.append(right_line_id)
        bounds[num] = bound


##找到两个面的公共交线
def get_line_id_by_twoSurfaces(surfaces, surface1_id, surface2_id):
    for line_id in surfaces[surface1_id].lines:
        if line_id in surfaces[surface2_id].lines:
            return line_id


##找到两个点的公共交线
def get_line_id_by_twoPoints(lines, p_id1, p_id2):
    for line in lines:
        if p_id1 in line.points and p_id2 in line.points:
            return line.l_id


def is_inSameLine(points, lines, p1_id, p2_id):
    for line_id in points[p1_id].lines:
        if p2_id in lines[line_id].points:
            return True
    return False

# def get_line_by_points(lines, p1, p2):
#     for line in lines:
#         if p1 in line.points and p2 in line.points:
#             return line.l_id
