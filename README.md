配置环境：vs2010+qt5.5.0（算法实现，从解释数据中推理得到知识图谱数据）
        :pycharm(分层网络绘制)
        
基本内容：构造解释的不确定性导致基于数据驱动的复杂地质构造建模技术难以满足实际生产应用需求。智能构造建模通过人机协同的方式从构造解释数据中挖掘和提取构造知识，结合解释数据形成知识引导的复杂构造建模新模式。本文针对复杂构造知识图谱构建问题开展研究，提出了基于知识推理的复杂构造知识图谱构建框架和方法。首先通过解释数据抽取复杂地质构造的元知识（比如层面和边界的相交关系、层位和断层的相交关系、断层和边界的相交关系等）并建立了交线、交点、层位、边界、断层之间的基础知识图谱。元知识仅仅描述了两个实体之间的关系，在此之上，利用规则识别交线和交线之间、交线和交点之间的关系，用于交线分割和交点融合，构建子面（利用交线对层位、断层、边界裁剪后形成的最小逻辑单元）逻辑边界的拓扑知识图谱。最后在子面知识图谱的基础上，识别和提取逻辑块的知识图谱。完成复杂构造知识图谱网络的构建。基于上述思想，形成了复杂构造知识图谱构建的流程。通过实际数据的应用测试，本方法能够适应各种复杂地质构造知识图谱的构建，有效支撑智能构造建模的应用。
![image](https://user-images.githubusercontent.com/101333374/229401407-6d70bf77-6c88-4ee0-a483-7c122aedcd45.png)
