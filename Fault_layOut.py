#######************断层面的布局#######


def fault_LayOut(blocks, surfaces, lines, points, bounds, layers, faults):
    ##断层存储相关信息
    for fault in faults:
        relate_surfaces = surfaces[fault.f_id].related_s
        for sur_id in relate_surfaces:
            if surfaces[sur_id].type == 2:
                fault.layers.append(sur_id)
            elif surfaces[sur_id].type == 1:
                fault.surfaces_bound.append(sur_id)

    ##将每个断层关联的层位的order按顺序存储
    for fault in faults:
        get_current_fault_related_layers_order(surfaces, fault)

    ###第一个层位面上的排序断层
    faults_order_by_layers = fault_order_on_currentLayer(blocks, surfaces, lines, points, bounds, layers, 0)
    for num in range(1, len(layers)):
        order_list_1 = fault_order_on_currentLayer(blocks, surfaces, lines, points, bounds, layers, num)
        ###将上一层位面的断层排序结果和下一层位面的断层排序结果合并，合并之后赋值给faults_order_by_layers
        faults_order_by_layers = merger_order_in_two_layers(faults_order_by_layers, order_list_1)
        # if len(faults_order_by_layers) == len(faults):
        #     break

    ###将faults里的断层的order属性赋值
    for num in range(0, len(faults_order_by_layers)):
        for fault in faults:
            if fault.f_id == faults_order_by_layers[num]:
                fault.order = num
                break

    ##将断层faults按照order排序，这样得到的faults的元素顺序就是和其order一致
    faults.sort(key=lambda t: t.order)

    ###将断层的其他属性内容补充完整，将断层上的点进行排序 TODO 代码还没完成，需要调试
    for fault in faults:
        current_fault_points_order(blocks, surfaces, lines, points, bounds, layers, fault)


"##将当前层位上的断层进行排序  layerOrder是当前层位"


def fault_order_on_currentLayer(blocks, surfaces, lines, points, bounds, layers, layerOrder):
    ""  ###首先按照断层与正面交线来排序""
    current_layer_fault_order = []  ###存放当前层位上断层从左到右的存放顺序
    current_layer = layers[layerOrder]
    current_layer_faults = current_layer.faults

    "##与正前方相交的断层相交"
    current_layer_front_fault_order = []  ###存放当前层位上断层在正面的从左到右的存放顺序
    layer_front_lines = []  ##当前层位和正面的交线
    front_surface_id = bounds[0].b_id
    for subsurface_id in current_layer.sublayers:
        for line_id in surfaces[subsurface_id].lines:
            if front_surface_id in lines[line_id].surfaces and line_id not in layer_front_lines:
                layer_front_lines.append(line_id)

    ##TODO 将左边界线的代码段提出去,可做可不做
    ##给layer_front_lines进行一个排序，从左到右
    layer_front_lines_ordered = []
    front_bound_lines = bounds[0].lines
    left_line_id = front_bound_lines[2]
    left_points = lines[left_line_id].points
    left_point_id = left_points[layerOrder + 1]  ##左边点

    "###将断层与正面的交线进行排序，同时排序与正面相交的断层面"
    right_point_id = -1
    while len(layer_front_lines_ordered) < len(layer_front_lines):
        "##由左端点找到断层面左边线的右端点"
        for line_id in layer_front_lines:
            if line_id in layer_front_lines_ordered:
                continue
            if left_point_id in lines[line_id].points:
                current_line_points = lines[line_id].points
                index = current_line_points.index(left_point_id)
                right_point_id = current_line_points[1 - index]
                layer_front_lines_ordered.append(line_id)
                break
        flag = True
        current_fault_id = -1
        "###找到right_point_id所在断层面"
        for surface_id in points[right_point_id].surfaces:
            if surface_id in current_layer_faults:
                current_layer_front_fault_order.append(surface_id)
                current_fault_id = surface_id
                break
        "###找到当前断层在正面且在当前层位面的下一条线段上的左点"
        for line_id in layer_front_lines:
            if line_id in layer_front_lines_ordered:
                continue
            if flag == False:
                break
            cross_points = lines[line_id].points
            for p_id in cross_points:
                if current_fault_id in points[p_id].surfaces:
                    left_point_id = p_id
                    flag = False
                    break

    if len(current_layer_front_fault_order) == len(current_layer_faults):
        current_layer.faults = current_layer_front_fault_order
        get_sublayers_order_by_faultOrder(current_layer, bounds, current_layer_front_fault_order, surfaces)
        return current_layer_front_fault_order

    "##与背面相交的断层相交"
    current_layer_back_fault_order = []  ###存放当前层位上断层从左到右的存放顺序
    layer_back_lines = []  ##当前层位和背面的交线
    back_surface_id = bounds[3].b_id

    for subsurface_id in current_layer.sublayers:
        for line_id in surfaces[subsurface_id].lines:
            if back_surface_id in lines[line_id].surfaces:
                layer_back_lines.append(line_id)
                break
    "##给layer_back_lines进行一个排序，从左到右"
    layer_back_lines_ordered = []
    back_bound_lines = bounds[3].lines
    left_line_id = back_bound_lines[3]
    left_points = lines[left_line_id].points
    left_point_id = left_points[layerOrder + 1]  ##左边点

    "###将背面的断层"
    right_point_id = -1
    while len(layer_back_lines_ordered) < len(layer_back_lines):
        "##由左端点找到断层面左边线的右端点"
        for line_id in layer_back_lines:
            if line_id in layer_back_lines_ordered:
                continue
            if left_point_id in lines[line_id].points:
                current_line_points = lines[line_id].points
                index = current_line_points.index(left_point_id)
                right_point_id = current_line_points[1 - index]
                layer_back_lines_ordered.append(line_id)
                break
        flag = True
        current_fault_id = -1
        "###找到right_point_id所在断层面"
        for surface_id in points[right_point_id].surfaces:
            if surface_id in current_layer_faults:
                current_layer_back_fault_order.append(surface_id)
                current_fault_id = surface_id
                break
        "###找到当前断层在正面且在当前层位面的下一条线段上的左点"
        for line_id in layer_back_lines:
            if line_id in layer_back_lines_ordered:
                continue
            if flag == False:
                break
            cross_points = lines[line_id].points
            for p_id in cross_points:
                if current_fault_id in points[p_id].surfaces:
                    left_point_id = p_id
                    flag = False
                    break

    "##按顺序存储"
    back_order_num = 0
    last_same_id = -1
    for surface_id in current_layer_front_fault_order:
        if surface_id in current_layer_back_fault_order:
            last_same_id = surface_id
            index = current_layer_back_fault_order.index(surface_id)
            for num in range(0, index):
                if current_layer_back_fault_order[num] not in current_layer_fault_order:
                    current_layer_fault_order.append(current_layer_back_fault_order[num])
                    back_order_num += 1
            back_order_num += 1  ##将当前点存储
        # else:  ##前面的断层在后面的断层
        current_layer_fault_order.append(surface_id)
    "##将背面没有排完序的接上"
    if back_order_num < len(current_layer_back_fault_order):
        index = current_layer_back_fault_order.index(last_same_id)
        for num in range(index + 1, len(current_layer_fault_order)):
            current_layer_fault_order.append(current_layer_back_fault_order[num])

    while len(current_layer_fault_order) < len(current_layer_faults):
        "##将与正面背面均不相交的断层面加入到排序中"
        left_surface_id = bounds[1].b_id
        for surface_id in current_layer_faults:
            if surface_id not in current_layer_fault_order:
                flag = False
                for num in range(0, len(current_layer_fault_order)):
                    right_surface_id = current_layer_fault_order[num]
                    ###判断当前断层在哪两个面之间
                    for block in blocks:
                        ##找到左右两个面都在的块
                        if left_surface_id in block.surfaces and right_surface_id in block.surfaces:
                            if surface_id in block.surfaces:
                                current_layer_fault_order.insert(num, surface_id)
                                flag = True
                                break
                    left_surface_id = right_surface_id
                if flag == False:
                    current_layer_fault_order.append(surface_id)
            else:
                continue

    current_layer.faults = current_layer_fault_order
    "###将该层位面子面进行排序"
    get_sublayers_order_by_faultOrder(current_layer, bounds, current_layer_fault_order, surfaces)
    return current_layer_fault_order


"##将两个有序列表按照顺序合并成一个列表"
def merger_order_in_two_layers(order_list_0, order_list_1):
    merge_order_list = []

    "##按顺序合并"
    back_order_num = 0
    last_same_id = -1  ##两个列表最后一个相同的元素id
    for surface_id in order_list_0:
        if surface_id in order_list_1:
            last_same_id = surface_id
            index = order_list_1.index(surface_id)
            for num in range(0, index):
                if order_list_1[num] not in merge_order_list:
                    merge_order_list.append(order_list_1[num])
                    back_order_num += 1
            back_order_num += 1  ##将当前点存储
        # else:  ##前面的断层在后面的断层
        merge_order_list.append(surface_id)
    "##将背面没有排完序的接上"
    if back_order_num < len(order_list_1):
        index = order_list_1.index(last_same_id)
        for num in range(index + 1, len(order_list_1)):
            merge_order_list.append(order_list_1[num])

    return merge_order_list


def get_fault_layer_points(surfaces, faults, fault_id, layers, layerOrder):
    current_fault_bound_points = []
    for fault in faults:
        if fault.f_id == fault_id:
            current_sublayers = []
            for relate_layer_id in fault.layers:
                if surfaces[relate_layer_id].order == layerOrder:
                    current_sublayers.append(relate_layer_id)
            if len(current_sublayers) == 2:
                index_0 = layers[layerOrder].sublayers.index(current_sublayers[0])
                index_1 = layers[layerOrder].sublayers.index(current_sublayers[1])
                if index_0 < index_1:
                    up_line_id = get_cross_line(surfaces, fault_id, current_sublayers[0])
                else:
                    up_line_id = get_cross_line(surfaces, fault_id, current_sublayers[1])
                index_2 = layers[layerOrder].sublayers.index(current_sublayers[0])
                index_3 = layers[layerOrder].sublayers.index(current_sublayers[1])
                if index_2 < index_3:
                    down_line_id = get_cross_line(surfaces, fault_id, current_sublayers[0])
                else:
                    up_line_id = get_cross_line(surfaces, fault_id, current_sublayers[1])


"###得到断层关联的层位order fault表示断层信息"


def get_current_fault_related_layers_order(surfaces, fault):
    for relate_layer_id in fault.layers:
        order = surfaces[relate_layer_id].order
        if order not in fault.relate_layer_order:
            fault.relate_layer_order.append(order)
    fault.relate_layer_order.sort(reverse=False)


###获得两个面的交线
def get_cross_line(surfaces, s_id_1, s_id_2):
    for line_id in surfaces[s_id_1].lines:
        if line_id in surfaces[s_id_2].lines:
            return line_id


"找到与正面背面都相交的断层面的点顺序，因为没有断完的断层面不影响拓扑块的邻接关系，所以就不用考虑点的排序"


def current_fault_points_order(blocks, surfaces, lines, points, bounds, layers, fault):
    "只有与两个边界面相交的断面需要考虑点的顺序，因为只有这种情况会影响块的拓扑关系。只与一个边界面相交或者不与边界面相交的断层面上的点只用按照层位面顺序就可"
    if len(fault.surfaces_bound) == 2:
        # if bounds[0].b_id in fault.surfaces_bound:
        ##断层面在正面的点
        fault_front_points = []
        for line_id in surfaces[fault.f_id].lines:
            if line_id in surfaces[bounds[0].b_id].lines:
                for p_id in lines[line_id].points:
                    if p_id not in fault_front_points:
                        fault_front_points.append(p_id)

        "##按照层位顺序将正面上的点左边右边分别存储"
        "当断层面与正面相交时，记录正面上的左右点，将断层面左边点依次按照层位顺序存储在left_points中，将将断层面右边点依次按照层位顺序存储在right_points中"
        for order in fault.relate_layer_order:
            current_order_fault_sublayers = []
            for sublayer_id in fault.layers:
                if surfaces[sublayer_id].order == order:
                    current_order_fault_sublayers.append(sublayer_id)
            current_order_sublayers = layers[order].sublayers
            index_0 = current_order_sublayers.index(current_order_fault_sublayers[0])
            index_1 = current_order_sublayers.index(current_order_fault_sublayers[1])
            if index_0 > index_1:
                temp = current_order_fault_sublayers[1]
                current_order_fault_sublayers[1] = current_order_fault_sublayers[0]
                current_order_fault_sublayers[0] = temp

            for point_id in fault_front_points:
                if current_order_fault_sublayers[0] in points[point_id].surfaces:
                    fault.left_points.append(point_id)
                elif current_order_fault_sublayers[1] in points[point_id].surfaces:
                    fault.right_points.append(point_id)

        "判断断层的倾角属性"
        "##如果断层只关联一个层位那么就都统一成锐角，正斜率,点顺序就可以随意点，直接将左点右点按顺序就可"
        if len(fault.relate_layer_order) == 1:
            fault.slipType = 0
            fault.order_points.append(fault.left_points[0])
            fault.order_points.append(fault.right_points[1])
        else:  ##断层关联的层位面有多个，那么可能会存在块关联关系影响点布局
            "###这里的块是指断面关联的层位面夹着的块,分别存储左边块和右边块"
            current_fault_left_blocks = []  ##存储断层面左边的块，按上下顺序存储
            current_fault_right_blocks = []  ##存储断层面右边的块，按上下顺序存储
            for num in range(0, len(fault.left_points) - 1):
                left_up_surface_id = get_sublayer_by_point(surfaces, points, fault.left_points[num])
                left_down_surface_id = get_sublayer_by_point(surfaces, points, fault.left_points[num + 1])
                left_block_id = get_public_block(blocks, left_up_surface_id, left_down_surface_id, fault.f_id)
                current_fault_left_blocks.append(left_block_id)

                right_up_surface_id = get_sublayer_by_point(surfaces, points, fault.right_points[num])
                right_down_surface_id = get_sublayer_by_point(surfaces, points, fault.right_points[num + 1])
                right_block_id = get_public_block(blocks, right_up_surface_id, right_down_surface_id, fault.f_id)
                current_fault_right_blocks.append(right_block_id)

            left_first_block_relateNum = 0
            right_first_block_relateNum = 0
            for block_id in blocks[current_fault_left_blocks[0]].related_b:
                if block_id in current_fault_right_blocks:
                    left_first_block_relateNum += 1
            for block_id in blocks[current_fault_right_blocks[0]].related_b:
                if block_id in current_fault_left_blocks:
                    right_first_block_relateNum += 1
            if fault.faultType == 0:  ###逆断层
                if left_first_block_relateNum <= right_first_block_relateNum:
                    fault.slipType = 0
                else:
                    fault.slipType = 1
            elif fault.faultType == 1:  ###逆断层
                if right_first_block_relateNum <= left_first_block_relateNum:
                    fault.slipType = 1
                else:
                    fault.slipType = 0
            # if left_first_block_relateNum <= 1:
            #     if fault.faultType == 0:  ###逆断层
            #         fault.slipType = 0
            #     elif fault.faultType == 1:  ###正断层
            #         fault.slipType = 1
            # elif right_first_block_relateNum <= 1:
            #     if fault.faultType == 0:  ###逆断层
            #         fault.slipType = 1
            #     elif fault.faultType == 1:  ###正断层
            #         fault.slipType = 0

            current_fault_order_points = []
            "####以左边块为基准,依次判断右边块在左边块的什么位置"
            if (fault.faultType == 0 and fault.slipType == 0) or (fault.faultType == 1 and fault.slipType == 1):
                for point_id in fault.left_points:
                    current_fault_order_points.append(point_id)
                flag = False
                for block_id in current_fault_right_blocks:
                    if flag:  ##表示右边块点已经全部按照顺序插入到左边点中，不用再继续循环了
                        break
                    block_id_index = current_fault_right_blocks.index(block_id)
                    relate_block_min_index = len(current_fault_left_blocks)  ###当前右块关联的左块最小的index
                    for relate_block_id in blocks[block_id].related_b:
                        if relate_block_id in current_fault_left_blocks:
                            index = current_fault_left_blocks.index(relate_block_id)
                            if relate_block_min_index > index:
                                relate_block_min_index = index
                    ###右边块与左边不邻接
                    if relate_block_min_index == len(current_fault_left_blocks):
                        for num in range(block_id_index, len(fault.right_points)):
                            current_fault_order_points.append(fault.right_points[num])
                        flag = True  ##表示右边块点已经全部按照顺序插入到左边点中
                        break
                    else:  ###右边块与左边邻接
                        # current_fault_blocks_order.insert(relate_block_min_index+1,block_id)
                        ##将右边块的第一个面排序  TODO 调试时注意一下，relate_block_min_index和index_0是否相等
                        index_0 = current_fault_order_points.index(fault.left_points[relate_block_min_index])
                        current_fault_order_points.insert(index_0 + 1, fault.right_points[block_id_index])
                for p_id in fault.right_points:
                    if p_id not in current_fault_order_points:
                        current_fault_order_points.append(p_id)
                ####以右边块为基准,将左边的点依次插入右边的点中
            elif (fault.faultType == 0 and fault.slipType == 1) or (fault.faultType == 1 and fault.slipType == 0):
                for point_id in fault.right_points:
                    current_fault_order_points.append(point_id)
                flag = False
                for block_id in current_fault_left_blocks:
                    if flag:  ##表示左边块点已经全部按照顺序插入到右边点中，不用再继续循环了
                        break
                    block_id_index = current_fault_left_blocks.index(block_id)
                    relate_block_min_index = len(current_fault_right_blocks)
                    for relate_block_id in blocks[block_id].related_b:
                        if relate_block_id in current_fault_right_blocks:
                            index = current_fault_right_blocks.index(relate_block_id)
                            if relate_block_min_index > index:
                                relate_block_min_index = index
                    ###左边块与右边块不邻接
                    if relate_block_min_index == len(current_fault_right_blocks):
                        for num in range(block_id_index, len(fault.left_points)):
                            current_fault_order_points.append(fault.left_points[num])
                        flag = True  ##表示右边块点已经全部按照顺序插入到左边点中
                        break
                    else:  ###左边块与右边邻接
                        ##将左边块的第一个面排序
                        index_0 = current_fault_order_points.index(fault.right_points[relate_block_min_index])
                        current_fault_order_points.insert(index_0 + 1, fault.left_points[block_id_index])
                for p_id in fault.left_points:
                    if p_id not in current_fault_order_points:
                        current_fault_order_points.append(p_id)
            fault.order_points = current_fault_order_points

    elif len(fault.surfaces_bound) == 1:
        # TODO 需要修改代码
        fault.slipType = 0
        if bounds[0].b_id in fault.surfaces_bound:  ##与正面相交
            ##和上面相同
            "首先找到当前断层面在正面上的点"
            fault_front_points = []  # ##断层面在正面的点
            for line_id in surfaces[fault.f_id].lines:
                if line_id in surfaces[bounds[0].b_id].lines:
                    for p_id in lines[line_id].points:
                        if p_id not in fault_front_points:
                            fault_front_points.append(p_id)

            ##按照层位顺序将正面上的点左边右边存储
            for order in fault.relate_layer_order:
                current_layer_order_points = []
                ###找到当前层位上的点
                for p_id in fault_front_points:
                    sublayer_order = surfaces[get_sublayer_by_point(surfaces, points, p_id)].order
                    if sublayer_order == order:
                        current_layer_order_points.append(p_id)  ###按层序存储，每一个层位上存放两个点
                ##将当前层位上的点进行排序，排序之后再添加到fault.order_points
                current_layer_order_points_order = []  ##存储当前层位上的点所属的线关联的断层面或边界面
                for p_id in current_layer_order_points:
                    front_line_id = -1  ###找到该点在正面以及层位面的线
                    for line_id in points[p_id].lines:
                        if front_line_id != -1:
                            break
                        for surface_id in lines[line_id].surfaces:
                            if surfaces[surface_id].type == 2:
                                if bounds[0].b_id in lines[line_id].surfaces:
                                    front_line_id = line_id
                                    break
                    ##找到该线，找到该线的另一个点
                    index = lines[front_line_id].points.index(p_id)
                    other_p_id = lines[front_line_id].points[1 - index]
                    ##找到该点所在的断层面id或者边界面id
                    for s_id in points[other_p_id].surfaces:
                        if surfaces[s_id].type == 3 or surfaces[s_id].type == 1:
                            current_layer_order_points_order.append(s_id)
                            break

                "##现在需要判断点的左右顺序"
                ##第一种情况，判断断层顺序
                if surfaces[current_layer_order_points_order[0]].type == 3 and surfaces[
                    current_layer_order_points_order[1]].type == 3:
                    if surfaces[current_layer_order_points_order[0]].order > surfaces[
                        current_layer_order_points_order[1]].order:
                        temp = current_layer_order_points[1]
                        current_layer_order_points[1] = current_layer_order_points[0]
                        current_layer_order_points[0] = temp
                elif surfaces[current_layer_order_points_order[0]].type == 3 and surfaces[
                    current_layer_order_points_order[1]].type == 1:
                    if surfaces[current_layer_order_points_order[1]].s_id == bounds[0].b_id:
                        temp = current_layer_order_points[1]
                        current_layer_order_points[1] = current_layer_order_points[0]
                        current_layer_order_points[0] = temp
                elif surfaces[current_layer_order_points_order[1]].type == 3 and surfaces[
                    current_layer_order_points_order[0]].type == 1:
                    if surfaces[current_layer_order_points_order[0]].s_id == bounds[2].b_id:
                        temp = current_layer_order_points[1]
                        current_layer_order_points[1] = current_layer_order_points[0]
                        current_layer_order_points[0] = temp

                for p_id in current_layer_order_points:
                    fault.order_points.append(p_id)

                fault.left_points.append(current_layer_order_points[0])
                fault.right_points.append(current_layer_order_points[1])

        elif bounds[3].b_id in fault.surfaces_bound:  ##与背面相交
            ##和上面相同
            "首先找到当前断层面在正面上的点"
            fault_back_points = []  # ##断层面在背面的点
            for line_id in surfaces[fault.f_id].lines:
                if line_id in surfaces[bounds[3].b_id].lines:
                    for p_id in lines[line_id].points:
                        if p_id not in fault_back_points:
                            fault_back_points.append(p_id)

            ##按照层位顺序将正面上的点左边右边存储
            for order in fault.relate_layer_order:
                current_layer_order_points = []
                ###找到当前层位上的点
                for p_id in fault_back_points:
                    sublayer_order = surfaces[get_sublayer_by_point(surfaces, points, p_id)].order
                    if sublayer_order == order:
                        current_layer_order_points.append(p_id)  ###按层序存储，每一个层位上存放两个点
                ##将当前层位上的点进行排序，排序之后再添加到fault.order_points
                current_layer_order_points_order = []  ##存储当前层位上的点所属的线关联的断层面或边界面
                for p_id in current_layer_order_points:
                    back_line_id = -1  ###找到该点在背面以及层位面的线
                    for line_id in points[p_id].lines:
                        if back_line_id != -1:
                            break
                        for surface_id in lines[line_id].surfaces:
                            if surfaces[surface_id].type == 2:
                                back_line_id = line_id
                                break
                    ##找到该线，找到该线的另一个点
                    index = lines[back_line_id].points.index(p_id)
                    other_p_id = lines[back_line_id].points[1 - index]
                    ##找到该点所在的断层面id或者边界面id
                    for s_id in points[other_p_id].surfaces:
                        if surfaces[s_id].type == 3 or surfaces[s_id].type == 1:
                            current_layer_order_points_order.append(s_id)
                            break

                "##现在需要判断点的左右顺序"
                ##第一种情况，判断断层顺序
                if surfaces[current_layer_order_points_order[0]].type == 3 and surfaces[
                    current_layer_order_points_order[1]].type == 3:
                    if surfaces[current_layer_order_points_order[0]].order > surfaces[
                        current_layer_order_points_order[1]].order:
                        temp = current_layer_order_points[1]
                        current_layer_order_points[1] = current_layer_order_points[0]
                        current_layer_order_points[0] = temp
                elif surfaces[current_layer_order_points_order[0]].type == 3 and surfaces[
                    current_layer_order_points_order[1]].type == 1:
                    if surfaces[current_layer_order_points_order[1]].s_id == bounds[0].b_id:
                        temp = current_layer_order_points[1]
                        current_layer_order_points[1] = current_layer_order_points[0]
                        current_layer_order_points[0] = temp
                elif surfaces[current_layer_order_points_order[1]].type == 3 and surfaces[
                    current_layer_order_points_order[0]].type == 1:
                    if surfaces[current_layer_order_points_order[0]].s_id == bounds[2].b_id:
                        temp = current_layer_order_points[1]
                        current_layer_order_points[1] = current_layer_order_points[0]
                        current_layer_order_points[0] = temp

                for p_id in current_layer_order_points:
                    fault.order_points.append(p_id)

                fault.left_points.append(current_layer_order_points[0])
                fault.right_points.append(current_layer_order_points[1])

                # ##判断当前层位上的点的左右点,找到该点所在的层位面，
                # # #层位面与正面的交线，另外一个点所在的面是断层面还是边界面，以此来判断当前点的左右点
                # for p_id in current_order_fault_points:
                #     sublayer_id = get_sublayer_by_point(surfaces, points, p_id)
                #     line_id = get_cross_line(lines, sublayer_id, bounds[0].b_id)
                #     p_id_index = lines[line_id].points.index(p_id)
                #     other_p_id = lines[line_id].points[1 - p_id_index]
                #     ###一个点是左边，另一个点在右边
                #     if fault.order == 0:
                #         if bounds[1].b_id in points[other_p_id].surfaces:
                #             fault.left_points.append(p_id)
                #             index = current_order_fault_points.index(p_id)
                #             fault.right_points.append(current_order_fault_points[1 - index])
                #             break
                #     else:
                #         for s_id in points[other_p_id].surfaces:
                #             if surfaces[s_id].type == 3:
                #                 if surfaces[s_id].order < fault.order:
                #                     fault.left_points.append(p_id)
                #                     index = current_order_fault_points.index(p_id)
                #                     fault.right_points.append(current_order_fault_points[1 - index])
                #                     break
                #         break
            # if fault.faultType == 0:  ##逆断层
            #     for num in range(0, len(fault.relate_layer_order)):
            #         fault.order_points.append(fault.left_points[num])
            #         fault.order_points.append(fault.right_points[num])
            # elif fault.faultType == 1:  ##正断层
            #     for num in range(0, len(fault.relate_layer_order)):
            #         fault.order_points.append(fault.right_points[num])
            #         fault.order_points.append(fault.left_points[num])

        elif bounds[3].b_id in fault.surfaces_bound:  ##与背面相交
            fault.slipType = 0
            fault_back_points = []
            for line_id in surfaces[fault.f_id]:
                if line_id in surfaces[bounds[3].b_id].lines:
                    for p_id in lines[line_id].points:
                        if p_id not in fault_back_points:
                            fault_back_points.append(p_id)

            ##按照层位顺序将正面上的点左边右边存储
            for order in fault.relate_layer_order:
                # current_order_fault_points = []
                ###找到当前层位上的点
                for p_id in fault_back_points:
                    sublayer_order = surfaces[get_sublayer_by_point(surfaces, points, p_id)].order
                    if sublayer_order == order:
                        fault.order_points.append(p_id)
                # ##判断当前层位上的点的左右点,找到该点所在的层位面，
                # # #层位面与正面的交线，另外一个点所在的面是断层面还是边界面，以此来判断当前点的左右点
                # for p_id in current_order_fault_points:
                #     sublayer_id = get_sublayer_by_point(surfaces, points, p_id)
                #     line_id = get_cross_line(lines, sublayer_id, bounds[0].b_id)
                #     p_id_index = lines[line_id].points.index(p_id)
                #     other_p_id = lines[line_id].points[1 - p_id_index]
                #     ###一个点是左边，另一个点在右边
                #     if fault.order == 0:
                #         if bounds[1].b_id in points[other_p_id].surfaces:
                #             fault.left_points.append(p_id)
                #             index = current_order_fault_points.index(p_id)
                #             fault.right_points.append(current_order_fault_points[1 - index])
                #             break
                #     else:
                #         for s_id in points[other_p_id].surfaces:
                #             if surfaces[s_id].type == 3:
                #                 if surfaces[s_id].order < fault.order:
                #                     fault.left_points.append(p_id)
                #                     index = current_order_fault_points.index(p_id)
                #                     fault.right_points.append(current_order_fault_points[1 - index])
                #                     break
                #         break
            #
            # if fault.faultType == 0:  ##逆断层
            #     for num in range(0, len(fault.relate_layer_order)):
            #         fault.order_points.append(fault.left_points[num])
            #         fault.order_points.append(fault.right_points[num])
            # elif fault.faultType == 1:  ##正断层
            #     for num in range(0, len(fault.relate_layer_order)):
            #         fault.order_points.append(fault.right_points[num])
            #         fault.order_points.append(fault.left_points[num])
    ##TODO 目前这个先不写
    elif len(fault.surfaces_bound) == 0:  ##按层序来排序
        ##找到当前断层面的在层位面上的线，那么其余的线就是在投影在投影面上的线
        ##找到投影线，随机选一个做在前面，后一条线在后面。这样存一个面的线就可以了
        fault.slipType = 0
        first_line_id = -1
        for line_id in surfaces[fault.f_id].lines:
            if len(lines[line_id].surfaces) == 1:
                first_line_id = line_id
                break
        for order in fault.relate_layer_order:
            # current_order_fault_points = []
            ###找到当前层位上的点
            for p_id in lines[first_line_id].points:
                sublayer_order = surfaces[get_sublayer_by_point(surfaces, points, p_id)].order
                if sublayer_order == order:
                    fault.order_points.append(p_id)
    #     current_fault_points = []  ##存储当前断层的点，只存储一边的点
    #     for line_id in surfaces[fault.f_id].lines:
    #         for point_id in lines[line_id].points:
    #             if point_id not in current_fault_points:
    #                 current_fault_points.append(point_id)
    #
    #     ##如果断层只与一个层位面相关联，那么就不需要考虑
    #     if len(fault.relate_layer_order)==1:
    #
    #
    #     ##按照层位顺序将正面上的点左边右边存储
    #     for order in fault.relate_layer_order:
    #         current_order_fault_points = []
    #         ###找到当前层位上的点
    #         for p_id in current_fault_points:
    #             sublayer_order = surfaces[get_sublayer_by_point(surfaces, points, p_id)].order
    #             if sublayer_order == order:
    #                 current_order_fault_points.append(p_id)
    #         ###将当前层位上的点分成两类


###找到三个面共同有的块
def get_public_block(blocks, s_id_1, s_id_2, s_id_3):
    for block in blocks:
        if s_id_1 in block.surfaces and s_id_2 in block.surfaces and s_id_3 in block.surfaces:
            return block.b_id


###找到断层上的点所在的层位面id
def get_sublayer_by_point(surfaces, points, point_id):
    for surface_id in points[point_id].surfaces:
        if surfaces[surface_id].type == 2:
            return surface_id


def get_cross_line(lines, s_id_1, s_id_2):
    for line in lines:
        if s_id_1 in line.surfaces and s_id_2 in line.surfaces:
            return line.l_id


def get_sublayers_order_by_faultOrder(current_layer, bounds, current_layer_fault_order, surfaces):
    ##将层位上的子面按照断层顺序从左到右排列
    current_layer_sublayer_order = []
    left_surface_id = bounds[1].b_id
    for num in range(0, len(current_layer_fault_order)):
        right_surface_id = current_layer_fault_order[num]
        for layer_id in current_layer.sublayers:
            if layer_id not in current_layer_sublayer_order:
                if left_surface_id in surfaces[layer_id].related_s and right_surface_id in surfaces[layer_id].related_s:
                    current_layer_sublayer_order.append(layer_id)
                    left_surface_id = right_surface_id
                    break
    ##将未排序的层位面子面存储
    for layer_id in current_layer.sublayers:
        if layer_id not in current_layer_sublayer_order:
            current_layer_sublayer_order.append(layer_id)
    ##当前层位面的子面换成有序子面
    current_layer.sublayers = current_layer_sublayer_order

    ###将当前层位面关联的断层面换成有序断层面
    # current_layer.faults = current_layer_fault_order
