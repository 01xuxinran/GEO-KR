###层位布局

import version5.Struct_5 as st5


def layers_location(blocks, surfaces, lines, points, layers, up_down_surfaces, bounds):
    # front_surface_id = bounds[0].b_id  ##正面的id
    up_surface_id = up_down_surfaces[0].s_id  ##顶面的id

    ##将层位上的子面存储
    already_surfaces_id = []
    belong_block = surfaces[up_surface_id].blocks
    belong_block_id = belong_block[0]

    firstLayer = st5.layer(0)
    for surface_id in blocks[belong_block_id].surfaces:
        if surfaces[surface_id].type == 2:
            if surface_id not in firstLayer.sublayers:
                surfaces[surface_id].order = 0
                firstLayer.sublayers.append(surface_id)
                already_surfaces_id.append(surface_id)
    layers[0] = firstLayer

    for num in range(1, len(layers)):
        layer = st5.layer(num)
        for surface_id in layers[num - 1].sublayers:
            belong_blocks_id = surfaces[surface_id].blocks
            for block_id in belong_blocks_id:
                for subject_surface_id in blocks[block_id].surfaces:
                    if surfaces[subject_surface_id].type == 2 and subject_surface_id not in already_surfaces_id:
                        surfaces[subject_surface_id].order = num
                        layer.sublayers.append(subject_surface_id)
                        already_surfaces_id.append(subject_surface_id)
        layers[num] = layer

    ##将层位关联的断层存储
    for layer in layers:
        sublayers = layer.sublayers
        for sublayer_id in sublayers:
            for relate_surface_id in surfaces[sublayer_id].related_s:
                if surfaces[relate_surface_id].type == 3 and relate_surface_id not in layer.faults:
                    layer.faults.append(relate_surface_id)

    ##将边界面上的边界点跟层位面上的点联系在一起
    bounds_lines = []  ##存储边界线
    for bound in bounds:
        subject_lines = bound.lines
        if subject_lines[2] not in bounds_lines:
            bounds_lines.append(subject_lines[2])
        if subject_lines[3] not in bounds_lines:
            bounds_lines.append(subject_lines[3])

    ###将每条边界线上的点进行排序
    for line_id in bounds_lines:
        unorder_points = lines[line_id].points
        order_points = []
        for num in range(0, len(unorder_points)):
            if num == 0:
                for point_id in unorder_points:
                    if up_down_surfaces[0].s_id in points[point_id].surfaces:
                        order_points.append(point_id)
                        break
            elif num < len(unorder_points) - 1:
                current_layer = layers[num - 1]
                flag = False
                for point_id in unorder_points:
                    if flag:
                        break
                    for sublayer_id in current_layer.sublayers:
                        if sublayer_id in points[point_id].surfaces and point_id not in order_points:
                            order_points.append(point_id)
                            flag = True
                            break
            elif num == len(unorder_points) - 1:
                for point_id in unorder_points:
                    if up_down_surfaces[1].s_id in points[point_id].surfaces:
                        order_points.append(point_id)
                        break
        lines[line_id].points = order_points

    depth_interval = 1 / (len(layers) + 1)
    ###边界上的点都已经排好序了，那么就要赋坐标值
    for line_id in bounds_lines:
        order_points = lines[line_id].points
        first_point_id = order_points[0]
        for num in range(1, len(order_points)):
            point_id = order_points[num]
            points[point_id].x = points[first_point_id].x
            points[point_id].y = points[first_point_id].y
            points[point_id].z = points[first_point_id].z - depth_interval * num
