class SpriteImage_Flipblock(SLib.SpriteImage_Static):  # XXX
    def __init__(self, parent):
        super().__init__(parent)

    @staticmethod
    def loadImages():
        if 'FlipBlockMiddle0' in ImageCache: return
        for i in range(16):
            ImageCache[f'FlipBlockLeft{i}'] = SLib.GetImg(f'flipblock_left_{i}.png')
            ImageCache[f'FlipBlockMiddle{i}'] = SLib.GetImg(f'flipblock_middle_{i}.png')
            ImageCache[f'FlipBlockEyes{i}'] = SLib.GetImg(f'flipblock_eyes_{i}.png')
            ImageCache[f'FlipBlockRight{i}'] = SLib.GetImg(f'flipblock_right_{i}.png')

    def dataChanged(self):
        width = ((self.parent.spritedata[2] & 0xF0) >> 4) + 1
        color = self.parent.spritedata[2] & 0xF

        left = ImageCache[f'FlipBlockLeft{color}']
        middle = ImageCache[f'FlipBlockMiddle{color}']
        eyes = ImageCache[f'FlipBlockEyes{color}']
        right = ImageCache[f'FlipBlockRight{color}']

        image = QtGui.QPixmap(width * 24, 24)
        image.fill(QtCore.Qt.transparent)

        for w in range(left.width(), image.width() - right.width(), middle.width()):
            image_painter = QtGui.QPainter(image)
            image_painter.drawPixmap(w, 0, middle)
            image_painter.end()

        image_painter = QtGui.QPainter(image)
        image_painter.drawPixmap(0, 0, left)
        image_painter.drawPixmap(image.width() - right.width(), 0, right)

        image_painter.drawPixmap(int((image.width() / 2) - (eyes.width() / 2)), 0, eyes)

        image_painter.end()

        self.image = image

        self.offset = (8 - (width / 2) * 16, 0)

        super().dataChanged()