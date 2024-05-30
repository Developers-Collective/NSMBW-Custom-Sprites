class SpriteImage_DryBonesNew(SLib.SpriteImage_StaticMultiple):  # XXX
    def __init__(self, parent):
        super().__init__(parent)
        self.offset = (-3, -16)

    @staticmethod
    def loadImages():
        if 'DryBonesNew0' in ImageCache: return
        for i in range(16):
            ImageCache[f'DryBonesNew{i}'] = SLib.GetImg(f'dry_bones/normal/{i}.png')

    def dataChanged(self):
        color = (self.parent.spritedata[2] >> 4) & 0xF
        size = (self.parent.spritedata[2] >> 2) & 0x3

        s = f'DryBonesNew{color}'
        img = ImageCache[s] if ImageCache[s] else ImageCache[f'DryBonesNew0']
        self.offset = (-3, -16)

        if size == 0:
            img = img.transformed(QtGui.QTransform().scale(0.5, 0.5))

        elif size == 1:
            img = img.transformed(QtGui.QTransform().scale(0.75, 0.75))
            self.offset = (
                self.offset[0] - (img.width() / 6),
                self.offset[1] - (img.height() / 4.5)
            )

        else:
            img = img.transformed(QtGui.QTransform().scale(1.0, 1.0))
            self.offset = (
                self.offset[0] - (img.width() / 6),
                self.offset[1] - (img.height() / 3)
            )

        self.image = img

        super().dataChanged()



class SpriteImage_DryBonesWing(SLib.SpriteImage_StaticMultiple):  # YYY
    def __init__(self, parent):
        super().__init__(parent)
        self.offset = (-4, -16)

        self.aux.append(SLib.AuxiliaryTrackObject(parent, 0, 0, SLib.AuxiliaryTrackObject.Horizontal))
        self.aux.append(SLib.AuxiliaryTrackObject(parent, 0, 0, SLib.AuxiliaryTrackObject.Vertical))

    @staticmethod
    def loadImages():
        if 'DryBonesWing0' in ImageCache: return
        for i in range(16):
            ImageCache[f'DryBonesWing{i}'] = SLib.GetImg(f'dry_bones/wing/{i}.png')

    def dataChanged(self):
        color = (self.parent.spritedata[2] >> 4) & 0xF
        size = (self.parent.spritedata[2] >> 2) & 0x3
        is_moving_vertically = bool((self.parent.spritedata[2] >> 1) & 0x1)
        distance = (self.parent.spritedata[3] & 0xFF) * 16 + 16

        s = f'DryBonesWing{color}'
        img = ImageCache[s] if ImageCache[s] else ImageCache[f'DryBonesWing0']

        self.offset = (-4, -16)

        if size == 0:
            img = img.transformed(QtGui.QTransform().scale(0.5, 0.5))

        elif size == 1:
            img = img.transformed(QtGui.QTransform().scale(0.75, 0.75))
            self.offset = (
                self.offset[0] - (img.width() / 6),
                self.offset[1] - (img.height() / 4.5)
            )

        else:
            img = img.transformed(QtGui.QTransform().scale(1.0, 1.0))
            self.offset = (
                self.offset[0] - (img.width() / 6),
                self.offset[1] - (img.height() / 3)
            )

        self.image = img

        if is_moving_vertically:
            self.aux[0].setSize(0, 0)
            self.aux[0].setPos(self.offset[0], 0)

            self.aux[1].setSize(8, distance)
            self.aux[1].setPos((-self.offset[0] + 4) * 1.5, (-distance // 2 + 7) * 1.5 + 14)

        else:
            self.aux[0].setSize(distance, 8)
            self.aux[0].setPos((-distance // 2 - self.offset[0] + 8) * 1.5, 16)

            self.aux[1].setSize(0, 0)
            self.aux[1].setPos(self.offset[0], 0)

        super().dataChanged()