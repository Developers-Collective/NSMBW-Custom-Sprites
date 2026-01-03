class SpriteImage_WaterGeyser(SLib.SpriteImage_StaticMultiple):  # XXX
    def __init__(self, parent):
        super().__init__(parent)

    @staticmethod
    def loadImages():
        if 'WaterGeyserTop0' in ImageCache: return
        for i in range(1):
            ImageCache[f'WaterGeyserTop{i}'] = SLib.GetImg(f'water_geyser_top{i}.png')
            ImageCache[f'WaterGeyserMiddle{i}'] = SLib.GetImg(f'water_geyser_middle{i}.png')

    def dataChanged(self):
        color = (self.parent.spritedata[2] & 0xF) % 1

        height = (self.parent.spritedata[4] & 0xFF) + 6
        width = 0.25 + ((self.parent.spritedata[3] & 0xF) * 0.25)
        if width == 0.25: width = 1

        base_width = 156

        top = ImageCache[f'WaterGeyserTop{color}']
        middle = ImageCache[f'WaterGeyserMiddle{color}']

        image = QtGui.QPixmap(top.width(), height * 24)
        image.fill(QtCore.Qt.transparent)
        for i in range(height):
            image_painter = QtGui.QPainter(image)
            image_painter.drawPixmap(0, i * 24, middle)
            image_painter.end()

        image_painter = QtGui.QPainter(image)
        image_painter.drawPixmap(0, 0, top)
        image_painter.end()

        image = image.scaled(int(base_width * width), image.height(), QtCore.Qt.IgnoreAspectRatio, QtCore.Qt.SmoothTransformation)
        self.image = image

        self.offset = (((-self.image.width() / 1.5) // 2) + 8, (-self.image.height() // 1.5) + 16)

        super().dataChanged()