####布局所有点坐标

"""按照左边界线上的点均分，也就是层位均分，从左到右，
遇到一个断层，若断层上的点没有任何约束，那么就让其与相应层位深度保持一致，
如果断层上的点已经有约束，那么就要利用这种约束
没有断完的断层，按照师姐的处理办法，面是不变的，只将一个点向下移动一定的断距
点在z轴上的值需要根据与其相连的左边的点来确定，x值由每个断层动态决定，因为有z值在约束。
根据断层的个数将每个断层约束在一定范围内
"""
"坐标x沿从左到右方向为正向，y从前到后方向为正向，z从下到上方向为正向"


def get_location(blocks, surfaces, lines, points, bounds, layers, faults):
    # depth = 1 / (len(layers) + 1)
    width = 1 / (len(faults))

    already_locate_points = []  ##存储已经分配好坐标的点，顶面和底面的点已经分配好，不需要添加在里面
    left_front_line_id = bounds[0].lines[2]
    left_back_line_id = bounds[3].lines[3]

    left_front_points = lines[left_front_line_id].points
    left_back_points = lines[left_back_line_id].points

    ##该类中只考虑层位面和断层面上的点
    for num in range(1, len(left_front_points) - 1):
        already_locate_points.append(left_front_points[num])
        already_locate_points.append(left_back_points[num])
    #
    # for layer in layers:
    #     current_layer_front_points = get_back_points_layers(surfaces, lines, bounds, layers, layer)
    #     front_points_layers.append(current_layer_front_points)
    #
    for fault in faults:
        index = faults.index(fault)  ##也就是fault的order
        ##当前断层上的点之间的间隔width_offset
        width_offset = width / (len(fault.relate_layer_order) * 2 + 1)
        X_max = (index + 1) * width - width_offset
        X_min = index * width + width_offset
        min_order = fault.relate_layer_order[0]
        max_order = fault.relate_layer_order[len(fault.relate_layer_order) - 1]
        if len(fault.surfaces_bound) == 2:
            # Z_max = depth * (max_order + 1) + depth_offset
            # 每个断层面在正面上的点，左边点坐标都是可以确定的，右边的点是左边两个点坐标的中间点
            first_point_id = fault.order_points[0]
            last_point_id = fault.order_points[len(fault.order_points) - 1]

            """接下来只用考虑x和z轴坐标"""
            if fault.faultType == 0 and fault.slipType == 0:  ##逆断层并且是断层面倾角为锐角
                ###首先把断层与正面的交线上的两端点坐标确定下来
                points[first_point_id].x = X_max
                points[last_point_id].x = X_min

                ###将左边点的z轴赋值
                for p_id in fault.left_points:
                    left_point_id = get_left_relate_point_id(surfaces, lines, points, bounds, p_id)
                    if left_point_id in already_locate_points:
                        points[p_id].z = points[left_point_id].z
                ###获得左边最后一个点的z坐标
                left_last_point_id = fault.left_points[len(fault.left_points) - 1]
                left_last_index = fault.order_points.index(left_last_point_id)  ##左边最后一个点在所有点上的index
                # last_order = fault.relate_layer_order[len(fault.relate_layer_order) - 1]  ##左边点最后一个点所在的层位order
                ##左边最后一个点距离z=0的距离  TODO 需要注意一下
                distance = points[left_last_point_id].z
                ###distance_interval是左边最后一个点之后的右边点之间的z间距
                distance_interval = distance / (len(fault.order_points) - left_last_index)  ###点在z轴的偏移深度

                ###获得左边最后一个点之后的所有点的z值
                for num in range(left_last_index + 1, len(fault.order_points)):
                    points[fault.order_points[num]].z = points[left_last_point_id].z - distance_interval * (
                            num - left_last_index)

                ##得到断层面的斜率
                K = (points[last_point_id].z - points[first_point_id].z) / (
                        points[last_point_id].x - points[first_point_id].x)

                ###由斜率以及左边点的z值，获得左边点的x值
                for p_id in fault.left_points:
                    points[p_id].x = points[first_point_id].x + (points[p_id].z - points[first_point_id].z) / K

                ##右边点位于左边点之间的均分左边点坐标
                for num in range(0, len(fault.left_points) - 1):
                    up_point_id = fault.left_points[num]
                    down_point_id = fault.left_points[num + 1]

                    ###找到这两个点之间的在右边的点，然后按点数均分这两个点坐标
                    up_index = fault.order_points.index(up_point_id)
                    down_index = fault.order_points.index(down_point_id)
                    point_count = down_index - up_index

                    if point_count == 1:
                        continue
                    else:
                        ###由左边点来获得右边点，每两个左边点之间的右边点均分两左边点坐标
                        z_distance_interval = (points[up_point_id].z - points[down_point_id].z) / point_count
                        x_distance_interval = (points[up_point_id].x - points[down_point_id].x) / point_count
                        for p_index in range(up_index + 1, down_index):
                            p_id = fault.order_points[p_index]
                            points[p_id].z = points[up_point_id].z - z_distance_interval * (p_index - up_index)
                            points[p_id].x = points[up_point_id].x - x_distance_interval * (p_index - up_index)

                ###左边点最后一个点之后的所有点的x值
                for num in range(left_last_index + 1, len(fault.order_points)):
                    p_id = fault.order_points[num]
                    points[p_id].x = points[left_last_point_id].x - distance_interval * (
                            num - left_last_index) / K

                # # 将正面上所有点的y值赋坐标0
                # for p_id in fault.order_points:
                #     points[p_id].y = 0
                #     already_locate_points.append(p_id)
                #
                # "将正面对应的背面的点作处理"
                # ###找到正面点对应的后面点
                # for front_p_id in fault.order_points:
                #     back_p_id = get_front_p_opposite_back_p(surfaces, lines, points, bounds, fault, front_p_id)
                #     points[back_p_id].x = points[front_p_id].x
                #     points[back_p_id].y = 1
                #     points[back_p_id].z = points[front_p_id].z
                #     already_locate_points.append(back_p_id)

            elif fault.faultType == 0 and fault.slipType == 1:  ##逆断层并且是断层面倾角为钝角
                ###首先把断层与正面的交线上的两端点坐标确定下来

                points[first_point_id].x = X_min  ##右边第一个点
                points[last_point_id].x = X_max  ##左边最后一个点

                ###将左边点的z轴赋值
                for p_id in fault.left_points:
                    # points[p_id].y = 0
                    left_point_id = get_left_relate_point_id(surfaces, lines, points, bounds, p_id)
                    # if left_point_id in already_locate_points:
                    points[p_id].z = points[left_point_id].z
                ###左边第一个点之前的点z值均分一下
                left_first_point_id = fault.left_points[0]
                left_first_index = fault.order_points.index(left_first_point_id)  ##左边第一个点在所有点上的index
                first_order = fault.relate_layer_order[0]  ##左边点最后一个点所在的层位order
                distance = points[left_front_points[first_order]].z - points[
                    left_first_point_id].z  ##左边界线上前一个z值减去左边第一个点的z值
                ###distance_interval是左边最后一个点之后的右边点之间的z间距
                distance_interval = distance / (left_first_index + 1)  ###点在z轴的偏移深度

                ###获得左边第一个点之前的所有点的z值
                for num in range(0, left_first_index):
                    points[fault.order_points[num]].z = points[left_first_point_id].z + distance_interval * (
                            left_first_index - num)

                ##得到断层面的斜率
                K = (points[last_point_id].z - points[first_point_id].z) / (
                        points[last_point_id].x - points[first_point_id].x)

                ###由斜率以及左边点的z值，获得左边点的x值
                for p_id in fault.left_points:
                    points[p_id].x = points[first_point_id].x + (points[p_id].z - points[first_point_id].z) / K

                ##左边点之上的的右边点赋值x坐标
                for num in range(0, left_first_index):
                    p_id = fault.order_points[num]
                    points[p_id].x = points[first_point_id].x + (points[p_id].z - points[first_point_id].z) / K

                ##右边点位于左边点之间的均分左边点坐标
                for num in range(0, len(fault.left_points) - 1):
                    up_point_id = fault.left_points[num]
                    down_point_id = fault.left_points[num + 1]

                    ###找到这两个点之间的在右边的点，然后按点数均分这两个点坐标
                    up_index = fault.order_points.index(up_point_id)
                    down_index = fault.order_points.index(down_point_id)

                    point_count = down_index - up_index
                    if point_count == 1:
                        continue
                    else:
                        ###由左边点来获得右边点，每两个左边点之间的右边点均分两左边点坐标
                        z_distance_interval = (points[up_point_id].z - points[down_point_id].z) / point_count
                        x_distance_interval = (points[up_point_id].x - points[down_point_id].x) / point_count
                        for p_index in range(up_index + 1, down_index):
                            p_id = fault.order_points[p_index]

                            points[p_id].z = points[up_point_id].z - z_distance_interval * (p_index - up_index)
                            points[p_id].x = points[up_point_id].x - x_distance_interval * (p_index - up_index)

                # # 将正面上所有点的y值赋坐标0
                # for p_id in fault.order_points:
                #     points[p_id].y = 0
                #     already_locate_points.append(p_id)
                #
                # ###找到正面点对应的后面点
                # for front_p_id in fault.order_points:
                #     back_p_id = get_front_p_opposite_back_p(surfaces, lines, points, bounds, fault,
                #                                             front_p_id)
                #     points[back_p_id].x = points[front_p_id].x
                #     points[back_p_id].y = 1
                #     points[back_p_id].z = points[front_p_id].z
                #
                #     ###将断层面上的点存储起来
                #     for p in points:
                #         if fault.f_id in p.surfaces:
                #             if p.p_id not in already_locate_points:
                #                 already_locate_points.append(p.p_id)
            elif fault.faultType == 1 and fault.slipType == 0:  ##正断层并且是断层面倾角为锐角
                ###首先把断层与正面的交线上的两端点坐标确定下来
                points[first_point_id].x = X_max  ##右边第一个点
                points[last_point_id].x = X_min  ##左边最后一个点
                # points[first_point_id].y = 0
                # points[last_point_id].y = 0
                ###将左边点的z轴赋值
                for p_id in fault.left_points:
                    # points[p_id].y = 0
                    left_point_id = get_left_relate_point_id(surfaces, lines, points, bounds, p_id)
                    points[p_id].z = points[left_point_id].z
                ###左边第一个点之前的点z值均分一下
                left_first_point_id = fault.left_points[0]
                left_first_index = fault.order_points.index(left_first_point_id)  ##左边第一个点在所有点上的index
                first_order = fault.relate_layer_order[0]  ##左边点最后一个点所在的层位order
                distance = points[left_front_points[first_order]].z - points[left_first_point_id].z
                ###distance_interval是左边最后一个点之后的右边点之间的z间距
                distance_interval = distance / (left_first_index + 1)  ###点在z轴的偏移深度

                ###获得左边第一个点之前的所有点的z值
                for num in range(0, left_first_index):
                    fault.order_points[num].z = points[left_first_point_id].z + distance_interval * (
                            left_first_index - num)

                ##得到断层面的斜率
                K = (points[last_point_id].z - points[first_point_id].z) / (
                        points[last_point_id].x - points[first_point_id].x)

                ###由斜率以及左边点的z值，获得左边点的x值
                # for p_id in fault.left_points:
                #     points[p_id].x = points[first_point_id].x + (points[p_id].z - points[first_point_id].z) / K

                for num in range(0, len(fault.left_points) - 1):
                    p_id = fault.left_points[num]
                    points[p_id].x = points[first_point_id].x + (points[p_id].z - points[first_point_id].z) / K

                ##左边点之上的的右边点赋值x坐标
                for num in range(0, left_first_index):
                    p_id = fault.order_points[num]
                    points[p_id].x = points[first_point_id].x + (points[p_id].z - points[first_point_id].z) / K

                ##右边点位于左边点之间的均分左边点坐标
                for num in range(0, len(fault.left_points) - 1):
                    up_point_id = fault.left_points[num]
                    down_point_id = fault.left_points[num + 1]

                    ###找到这两个点之间的在右边的点，然后按点数均分这两个点坐标
                    up_index = fault.order_points.index(up_point_id)
                    down_index = fault.order_points.index(down_point_id)

                    point_count = down_index - up_index
                    if point_count == 1:
                        continue
                    else:
                        ###由左边点来获得右边点，每两个左边点之间的右边点均分两左边点坐标
                        z_distance_interval = (points[up_point_id].z - points[down_point_id].z) / point_count
                        x_distance_interval = (points[up_point_id].x - points[down_point_id].x) / point_count
                        for p_index in range(up_index + 1, down_index):
                            p_id = fault.order_points[p_index]

                            points[p_id].z = points[up_point_id].z - z_distance_interval * (p_index - up_index)
                            points[p_id].x = points[up_point_id].x - x_distance_interval * (p_index - up_index)
                ###找到正面点对应的后面点
                for front_p_id in fault.order_points:
                    back_p_id = get_front_p_opposite_back_p(surfaces, lines, points, bounds, fault,
                                                            front_p_id)
                    points[back_p_id].x = points[front_p_id].x
                    points[back_p_id].y = 1
                    points[back_p_id].z = points[front_p_id].z

                    # ###将断层面上的点存储起来
                    #                     # for p in points:
                    #                     #     if fault.f_id in p.surfaces:
                    #                     #         if p.p_id not in already_locate_points:
                    #                     #             already_locate_points.append(p.p_id)


            elif fault.faultType == 1 and fault.slipType == 1:  ##正断层并且是断层面倾角为钝角
                ###首先把断层与正面的交线上的两端点坐标确定下来
                # first_point_id = fault.order_points[0]
                # last_point_id = fault.order_points[len(fault.order_points) - 1]
                points[first_point_id].x = X_min
                points[last_point_id].x = X_max
                ###将左边点的z轴赋值
                for p_id in fault.left_points:
                    left_point_id = get_left_relate_point_id(surfaces, lines, points, bounds, p_id)
                    # if left_point_id in already_locate_points:
                    points[p_id].z = points[left_point_id].z
                ###获得左边最后一个点的z坐标
                left_last_point_id = fault.left_points[len(fault.left_points) - 1]
                left_last_index = fault.order_points.index(left_last_point_id)  ##左边最后一个点在所有点上的index
                # last_order = fault.relate_layer_order[len(fault.relate_layer_order) - 1]  ##左边点最后一个点所在的层位order
                distance = points[left_last_point_id].z
                ###distance_interval是左边最后一个点之后的右边点之间的z间距
                distance_interval = distance / (len(fault.order_points) - left_last_index)  ###点在z轴的偏移深度
                # distance_interval_x = (X_max-points[left_last_point_id].x) / (len(fault.order_points) - left_last_index)  ###点在z轴的偏移深度

                ###获得左边最后一个点之后的所有点的z和x值
                for num in range(left_last_index + 1, len(fault.order_points)):
                    p_id = fault.order_points[num]
                    points[p_id].z = points[left_last_point_id].z - distance_interval * (
                            num - left_last_index)

                ##得到断层面的斜率
                K = (points[last_point_id].z - points[first_point_id].z) / (
                        points[last_point_id].x - points[first_point_id].x)

                ###由斜率以及左边点的z值，获得左边点的x值
                for p_id in fault.left_points:
                    points[p_id].x = points[first_point_id].x + (points[p_id].z - points[first_point_id].z) / K

                ##右边点位于左边点之间的均分左边点坐标
                for num in range(0, len(fault.left_points) - 1):
                    up_point_id = fault.left_points[num]
                    down_point_id = fault.left_points[num + 1]

                    ###找到这两个点之间的在右边的点，然后按点数均分这两个点坐标
                    up_index = fault.order_points.index(up_point_id)
                    down_index = fault.order_points.index(down_point_id)

                    point_count = down_index - up_index
                    if point_count == 1:
                        continue
                    else:
                        ###由左边点来获得右边点，每两个左边点之间的右边点均分两左边点坐标
                        z_distance_interval = (points[up_point_id].z - points[down_point_id].z) / point_count
                        x_distance_interval = (points[down_point_id].x - points[up_point_id].x) / point_count
                        for p_index in range(up_index + 1, down_index):
                            p_id = fault.order_points[p_index]
                            # points[p_id].y = 0

                            points[p_id].z = points[up_point_id].z - z_distance_interval * (p_index - up_index)
                            points[p_id].x = points[up_point_id].x + x_distance_interval * (p_index - up_index)

                ###左边点最后一个点之后的所有点的x值
                for num in range(left_last_index + 1, len(fault.order_points)):
                    p_id = fault.order_points[num]
                    points[p_id].x = points[first_point_id].x + (points[p_id].z - points[first_point_id].z) / K

                # ###找到正面点对应的后面点
                # for front_p_id in fault.order_points:
                #     back_p_id = get_front_p_opposite_back_p(surfaces, lines, points, bounds, fault, front_p_id)
                #     points[back_p_id].x = points[front_p_id].x
                #     points[back_p_id].y = 1
                #     points[back_p_id].z = points[front_p_id].z
                #
                #     ###将断层面上的点存储起来
                #     for p in points:
                #         if fault.f_id in p.surfaces:
                #             if p.p_id not in already_locate_points:
                #                 already_locate_points.append(p.p_id)

            # 将正面上所有点的y值赋坐标0
            for p_id in fault.order_points:
                points[p_id].y = 0
                already_locate_points.append(p_id)

            "将正面对应的背面的点作处理"
            ###找到正面点对应的后面点
            for front_p_id in fault.order_points:
                back_p_id = get_front_p_opposite_back_p(surfaces, lines, points, bounds, fault,
                                                        front_p_id)
                points[back_p_id].x = points[front_p_id].x
                points[back_p_id].y = 1
                points[back_p_id].z = points[front_p_id].z
                already_locate_points.append(back_p_id)



        elif len(fault.surfaces_bound) == 1:
            first_point_id = fault.left_points[0]
            last_point_id = fault.left_points[len(fault.left_points) - 1]
            points[first_point_id].x = X_max
            points[last_point_id].x = X_min
            # width_offset = width / (len(fault.relate_layer_order) * 2 + 1)
            # X_max = (index + 1) * width - width_offset
            # X_min = index * width + width_offset

            "将断层面上的左边点坐标确定，右边点坐标和左边点一致"
            for p_id in fault.left_points:
                left_point_id = get_left_relate_point_id(surfaces, lines, points, bounds, p_id)
                points[p_id].z = points[left_point_id].z
            ##右边点z值和左边点z值一样
            for num in range(0, len(fault.right_points)):
                points[fault.right_points[num]].z = points[fault.left_points[num]].z

            if len(fault.relate_layer_order) == 1:
                for num in range(0, len(fault.right_points)):
                    # points[fault.right_points[num]].z = points[fault.left_points[num]].z
                    points[fault.right_points[num]].x = points[fault.left_points[num]].x
            else:
                ##得到断层面的斜率
                K = (points[last_point_id].z - points[first_point_id].z) / (
                        points[last_point_id].x - points[first_point_id].x)

                for num in range(1, len(fault.left_points) - 1):
                    left_point_id = fault.left_points[num]
                    points[left_point_id].x = points[first_point_id].x + (
                            points[left_point_id].z - points[first_point_id].z) / K

                ##右边点z值和左边点z值一样
                for num in range(0, len(fault.right_points)):
                    points[fault.right_points[num]].x = points[fault.left_points[num]].x

                ##y值取值不同而已
                if bounds[0].b_id in fault.surfaces_bound:
                    for num in range(0, len(fault.right_points)):
                        left_point_id = fault.left_points[num]
                        right_point_id = fault.right_points[num]
                        points[left_point_id].y = 0
                        points[right_point_id].y = 0
                elif bounds[3].b_id in fault.surfaces_bound:
                    for num in range(0, len(fault.right_points)):
                        left_point_id = fault.left_points[num]
                        right_point_id = fault.right_points[num]
                        points[left_point_id].y = 1
                        points[right_point_id].y = 1

            ##找到断层线上另外一个点
            for num in range(0, len(fault.right_points)):
                left_point_id = fault.left_points[num]
                order = fault.relate_layer_order[num]
                other_p_id = get_opposite_p(surfaces, points, fault, order)
                points[other_p_id].x = points[left_point_id].x
                points[other_p_id].y = 1 / 2
                points[other_p_id].z = points[left_point_id].z
                already_locate_points.append(other_p_id)

            # for p_id in fault.order_points:
            #     points[p_id].y = 0
            #
            # first_point_id = fault.order_points[0]
            # last_point_id = fault.order_points[len(fault.order_points) - 1]
            # points[first_point_id].x = X_max  ##第一个点
            # points[last_point_id].x = X_min  ##最后一个点
            # points[first_point_id].y = 0
            # points[last_point_id].y = 0
            # ##左边点在上，右边点在下
            # for p_id in fault.left_points:
            #     left_point_id = get_left_relate_point_id(surfaces, lines, points, bounds, p_id)
            #     if left_point_id in already_locate_points:
            #         points[p_id].z = points[left_point_id].z
            #
            # for num in range(0, len(fault.left_points) - 1):
            #     up_point_id = fault.left_points[num]
            #     down_point_id = fault.left_points[num + 1]
            #
            #     ###找到这两个点之间的在右边的点，然后按点数均分这两个点坐标
            #     up_index = fault.order_points.index(up_point_id)
            #     down_index = fault.order_points.index(down_point_id)
            #     ####TODO 没有考虑只交一个层位的情况
            #     ###由左边点来获得右边点，每两个左边点之间的右边点均分两左边点坐标
            #     z_distance_interval = points[down_point_id].z - points[up_point_id].z
            #     for p_index in range(up_index + 1, down_index):
            #         p_id = fault.order_points[p_index]
            #         points[p_id].y = 0
            #
            #         points[p_id].z = points[up_point_id].z + z_distance_interval * (p_index - up_index)
            #         points[p_id].x = points[up_point_id].x + z_distance_interval * (p_index - up_index) / K

        elif len(fault.surfaces_bound) == 0:  ##TODO 以后再补充
            current_fault_points = []
            for line_id in surfaces[fault.f_id].lines:
                for p_id in lines[line_id].points:
                    if p_id not in current_fault_points:
                        current_fault_points.append(p_id)

            X = (X_max + X_min) / 2
            for p_id in current_fault_points:
                points[p_id].x = X
                ###TODO 没有左边的点，应该根据该断层面左右两边的面来确定
                left_point_id = get_left_relate_point_id(surfaces, lines, points, bounds, p_id)
                points[p_id].z = points[left_point_id].z

    ##TODO 需要将右边界线上的点与关联的左边点z值保持一致
    right_front_line_id = bounds[0].lines[3]
    right_back_line_id = bounds[3].lines[2]

    right_front_points = lines[right_front_line_id].points
    right_back_points = lines[right_back_line_id].points


    for num in range(1, len(right_front_points) - 1):
        right_front_point_id = right_front_points[num]
        left_relate_point_id= get_left_relate_point_id(surfaces, lines, points, bounds, right_front_point_id)
        points[right_front_point_id].z=points[left_relate_point_id].z

    for num in range(1, len(right_back_points) - 1):
        right_back_point_id = right_back_points[num]
        left_relate_point_id = get_left_relate_point_id(surfaces, lines, points, bounds, right_back_point_id)
        points[right_back_point_id].z = points[left_relate_point_id].z



##断完了的断层面在边界面的交点，找到与该交点在同一条线上的该层位上的另一个点id
def get_left_relate_point_id(surfaces, lines, points, bounds, p_id):
    if bounds[0].b_id in points[p_id].surfaces:
        for line_id in points[p_id].lines:
            if bounds[0].b_id in lines[line_id].surfaces:
                for s_id in lines[line_id].surfaces:
                    if surfaces[s_id].type == 2:
                        ##找到了这样的线
                        index = lines[line_id].points.index(p_id)
                        other_p_id = lines[line_id].points[1 - index]
                        return other_p_id
    elif bounds[3].b_id in points[p_id].surfaces:
        for line_id in points[p_id].lines:
            if bounds[3].b_id in lines[line_id].surfaces:
                for s_id in lines[line_id].surfaces:
                    if surfaces[s_id].type == 2:
                        ##找到了这样的线
                        index = lines[line_id].points.index(p_id)
                        other_p_id = lines[line_id].points[1 - index]
                        return other_p_id


##获得断层面正面对应的背面交点
def get_front_point_re_back_point(surfaces, lines, fault, point_id):
    for line_id in surfaces[fault.f_id].lines:
        for surface_id in lines[line_id].surfaces:
            if surfaces[surface_id].type == 2:
                if point_id in lines[line_id].points:
                    for p_id in lines[line_id].points:
                        if p_id != point_id:
                            return p_id


##找到当前层位与正面的交点
def get_front_points_layer(surfaces, lines, bounds, layers, layer):
    front_points_layers_current = []
    left_line_id = bounds[0].lines[2]
    left_line_points = lines[left_line_id].points
    front_points_layers_current.append(left_line_points[layer.order + 1])  ##最左边的点
    front_lines_layers_current = []

    for sublayer_id in layers.sublayers:
        for line_id in surfaces[sublayer_id].lines:
            if line_id in surfaces[bounds[0].b_id].lines:
                front_lines_layers_current.append(line_id)

    for line_id in front_lines_layers_current:
        for point_id in lines[line_id].points:
            if point_id not in front_points_layers_current:
                front_points_layers_current.append(point_id)
    return front_points_layers_current


def get_back_points_layers(surfaces, lines, bounds, layers, layer):
    back_points_layers_current = []
    left_line_id = bounds[3].lines[3]
    left_line_points = lines[left_line_id].points
    back_points_layers_current.append(left_line_points[layer.order + 1])  ##最左边的点
    front_lines_layers_current = []

    for sublayer_id in layers.sublayers:
        for line_id in surfaces[sublayer_id].lines:
            if line_id in surfaces[bounds[3].b_id].lines:
                front_lines_layers_current.append(line_id)

    for line_id in front_lines_layers_current:
        for point_id in lines[line_id].points:
            if point_id not in back_points_layers_current:
                back_points_layers_current.append(point_id)
    return back_points_layers_current


###获得断层上在正面上的点与之对应的背面上的点
def get_front_p_opposite_back_p(surfaces, lines, points, bounds, fault, front_p_id):
    for p in points:
        ##找到断层面背面的点
        if bounds[3].b_id in p.surfaces and fault.f_id in p.surfaces:
            for line_id in surfaces[fault.f_id].lines:
                if p.p_id in lines[line_id].points and front_p_id in lines[line_id].points:
                    return p.p_id


##找到只与一个边界面相交的断层面在区块中间对应的点id，num表示层位面order
def get_opposite_p(surfaces, points, fault, order):
    ##存储不在边界面上的点
    other_fault_points = []
    for p in points:
        if fault.f_id in p.surfaces and p.p_id not in fault.order_points:
            other_fault_points.append(p.p_id)

    for p_id in other_fault_points:
        for s_id in points[p_id].surfaces:
            if surfaces[s_id].type == 2 and surfaces[s_id].order == order:
                return p_id
