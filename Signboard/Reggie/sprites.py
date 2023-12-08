class SpriteImage_Signboard(SLib.SpriteImage_StaticMultiple):  # 310
    def __init__(self, parent):
        super().__init__(parent, 1.5)
        self.offset = (-8, -16)

    @staticmethod
    def loadImages():
        for i in range(9): # color
            for j in range(8): # direction
                ImageCache[f'Signboard{i}_{j}'] = SLib.GetImg(f'signboard{i}_{j}.png')

    def dataChanged(self):
        color = ((self.parent.spritedata[2] & 0xF0) >> 4) % 4
        direction = (self.parent.spritedata[2] & 0xF) % 8
        self.image = ImageCache[f'Signboard{color}_{direction}']

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
            -8 + (-16) * x,
            -8 + 16 * y,
        )

        super().dataChanged()