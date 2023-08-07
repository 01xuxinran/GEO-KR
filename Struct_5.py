class block:
    def __init__(self, b_id):
        self.b_id = b_id
        self.surfaces = []  ###只存储id 构成块的面的id
        self.related_b = []  ####存储关联的块id  关联的意思是有公共的面
        ##self.points=[] ##围城该块的点


class surface:
    def __init__(self, s_id):
        self.s_id = s_id
        self.lines = []  # 只存储id  构成面的线的id
        self.blocks = []  # 只存储id
        self.related_s = []  # 存储关联的面id 关联的意思是有公共的线
        self.type = -1  ##0表示顶或底，1表示边界面，2表示层位面，3表示断层面
        ### ##当是层位面时表示第几个层位面（从上而下排序），当是断层面时表示第几个断层面（从左到右排序）,当是边界面时从前左右后来排序,当是顶或底时，自顶向底排序
        self.order = -1
        self.faultType = -1  #####是断层面时 0表示逆断层，1表示正断层
        ###self.bound_points=[]   #之后需要搞一下的 TODO


# 线信息
class line:
    def __init__(self, l_id):
        self.l_id = l_id
        self.points = []  # 只存储id 构成线的点的id  边界线上的点从上到下排序
        self.surfaces = []  # 只存储id##########存储线所在的面id
        self.related_l = []  # 存储关联的线id 关联的意思是有公共的点


# 点信息
class point:
    def __init__(self, p_id, x=-1.00, y=-1.00, z=-1.00):
        self.p_id = p_id
        self.lines = []  # 所属的线
        self.surfaces = []  ####当前点所属的面
        ### self.blocks = []  ####当前点所属的块  后加的
        self.x = x
        self.y = y
        self.z = z


# 断层面信息，本版本不考虑断层只关联一个边界面的情况，考虑的都是断完的情况
class fault:
    def __init__(self, f_id):
        self.f_id = f_id
        self.layers = []  ####关联的层位面，存储id
        self.surfaces_bound = []  ####关联的边界面（新加的，因为断层面可能与后面相交或者不与后面相交。只存储投影面正面和后面  左边界和右边界不考虑）
        self.order = -1
        self.faultType = -1  #####是断层面时 0表示逆断层，1表示正断层   #后来加的，可以后面再看需不需要
        self.relate_layer_order = []  ###将关联层位的顺序存储
        self.left_points = []  ##一维列表，当前断层的左边点，每一个元素是断层与层位的交点及其order
        self.right_points = []  ##一维列表，当前断层的右边点
        ####一维列表，当前断层正面上的排好序的所有点 如果是断完的点那么存储的就是正面上的所有点，
        # 如果是只断了一个边界面，那么就是一个边界面上的点按照层序存储，不分左右边点。因为将同一层位上的两个点作为一个点
        # 如果与边界面都不相交，那么就将同一层位上的前后四个点按照前两个后两个的顺序排序。同时这里点前后其实是不确定的，自己可以确定谁在前谁在后
        self.order_points = []
        self.slipType = -1  ###0的时候斜率为正，1的时候斜率为负


##层位面信息(可能是子面)
class layer:
    def __init__(self, order):
        self.faults = []  ####关联的断层面  从左到右排列
        self.order = order  ##层位order 0表示从上到下的第一个层位
        self.sublayers = []  ##当前层位面所有子面 根据断层面顺序来依次从左到右排序


# 边界面信息
class bound:
    def __init__(self, order):
        self.b_id = -1
        self.order = order
        self.lines = []  ##只存储边界面上的边界线


##顶面或底面信息
class up_down_surface:
    def __init__(self, order):  ##order为0代表顶面，为1代表底面
        self.s_id = -1
        self.order = order
        self.ordered_lines = []
        self.points=[]


##存储可以表征面边界点的四个点
class surfaces_points:
    def __init__(self, num):  ##首先上下顶面再是边界面，再是断层面
        self.points = []  ###且点的顺序一定不能搞错
