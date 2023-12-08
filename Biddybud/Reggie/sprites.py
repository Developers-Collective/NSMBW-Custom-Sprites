class SpriteImage_Tenten(SLib.SpriteImage_StaticMultiple):  # XXX
    def __init__(self, parent):
        super().__init__(parent)

        self.aux.append(SLib.AuxiliaryTrackObject(parent, 0, 0, SLib.AuxiliaryTrackObject.Horizontal))

    @staticmethod
    def loadImages():
        if 'Tenten0_0' in ImageCache: return
        for i in range(1):
            for j in range(5):
                ImageCache[f'Tenten{i}_{j}'] = SLib.GetImg(f'tenten{i}_{j}.png')

    def dataChanged(self):
        spritetex_color = ((self.parent.spritedata[1] & 0xF0) >> 4) % 1
        color = (self.parent.spritedata[1] & 0xF) % 5
        facing_right = (self.parent.spritedata[0] & 0b100) >> 2
        distance = (self.parent.spritedata[3] & 0xFF) * 16 + 16

        self.image = ImageCache[f'Tenten{spritetex_color}_{color}'].copy()

        if facing_right:
            self.image = self.image.transformed(QtGui.QTransform().scale(-1, 1))

        self.offset = (-2 if facing_right else 0, 0)

        self.aux[0].setSize(distance, 8)
        self.aux[0].setPos((-distance // 2 - self.offset[0] + 8) * 1.5, 6)

        super().dataChanged()


class SpriteImage_TentenWing(SLib.SpriteImage_StaticMultiple):  # YYY
    def __init__(self, parent):
        super().__init__(parent)

        self.aux.append(SLib.AuxiliaryTrackObject(parent, 0, 0, SLib.AuxiliaryTrackObject.Horizontal))
        self.aux.append(SLib.AuxiliaryTrackObject(parent, 0, 0, SLib.AuxiliaryTrackObject.Vertical))

    @staticmethod
    def loadImages():
        if 'TentenWing0_0' in ImageCache: return
        for i in range(1):
            for j in range(5):
                ImageCache[f'TentenWing{i}_{j}'] = SLib.GetImg(f'tenten_wing{i}_{j}.png')
                ImageCache[f'TentenWingFront{i}_{j}'] = SLib.GetImg(f'tenten_wing_front{i}_{j}.png')

    def dataChanged(self):
        spritetex_color = ((self.parent.spritedata[1] & 0xF0) >> 4) % 1
        color = (self.parent.spritedata[1] & 0xF) % 5
        is_path = ((self.parent.spritedata[0] & 0b1000) >> 3) == 0
        up_down = (self.parent.spritedata[0] & 0b100) >> 2
        facing_right_up = (self.parent.spritedata[0] & 0b10) >> 1
        distance = ((self.parent.spritedata[3] & 0xF0) >> 4) * 16 + 16

        if is_path or (up_down and (not is_path)):
            self.image = ImageCache[f'TentenWingFront{spritetex_color}_{color}'].copy()
            self.offset = (-8, 0)

        else:
            self.image = ImageCache[f'TentenWing{spritetex_color}_{color}'].copy()

            if facing_right_up:
                self.image = self.image.transformed(QtGui.QTransform().scale(-1, 1))

            self.offset = (-10 if facing_right_up else 0, 0)

        if is_path:
            self.aux[0].setSize(0, 0)
            self.aux[0].setPos(self.offset[0], 0)

            self.aux[1].setSize(0, 0)
            self.aux[1].setPos(self.offset[0], 0)

        elif up_down:
            self.aux[0].setSize(0, 0)
            self.aux[0].setPos(self.offset[0], 0)

            self.aux[1].setSize(8, distance)
            self.aux[1].setPos((-self.offset[0] + 4) * 1.5, (-distance // 2 + 7) * 1.5)

        else:
            self.aux[0].setSize(distance, 8)
            self.aux[0].setPos((-distance // 2 - self.offset[0] + 8) * 1.5, 6)

            self.aux[1].setSize(0, 0)
            self.aux[1].setPos(self.offset[0], 0)

        super().dataChanged()