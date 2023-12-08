class SpriteImage_MidwayFlag(SLib.SpriteImage_StaticMultiple):  # 188
    def __init__(self, parent):
        super().__init__(parent)

    @staticmethod
    def loadImages():
        if 'MidwayFlag0' in ImageCache: return
        transform30 = QtGui.QTransform()
        transform30.rotate(330)
        for i in range(16):
            ImageCache[f'MidwayFlag{i}'] = SLib.GetImg(f'midway_flag_{i}.png')

        midwayflag = SLib.GetImg('midway_flag_0.png', True)
        if midwayflag is None: return

    def dataChanged(self):
        color = self.parent.spritedata[2] & 0xF
        self.image = ImageCache[f'MidwayFlag{color}']

        rotation = ((self.parent.spritedata[4] & 0xFF) << 8) + (self.parent.spritedata[5] & 0xFF)

        img_rotation = QtGui.QTransform()
        angle = rotation / 65520 * 360
        img_rotation.rotate(-angle)
        angle -= 90
        if angle < 0: angle += 360
        x, y = math.cos(angle * math.pi / 180), math.sin(angle * math.pi / 180)

        dimension = QtCore.QSize(self.image.width(), self.image.height())

        self.image = self.image.transformed(img_rotation)
        diff = QtCore.QSize(self.image.width() - dimension.width(), self.image.height() - dimension.height())
        self.image = self.image.copy(
            diff.width() // 2,
            diff.height() // 2,
            self.image.width() - (diff.width() // 2),
            self.image.height() - (diff.height() // 2)
        )

        self.offset = (
            -19 + (-27) * x,
            -19 + 27 * y,
        )

        super().dataChanged()