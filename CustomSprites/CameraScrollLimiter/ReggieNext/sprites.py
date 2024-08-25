class SpriteImage_CameraScrollLimiter(SLib.SpriteImage_StaticMultiple):  # XXX
    def __init__(self, parent):
        super().__init__(parent)
        self.offset = (0, 0)

    @staticmethod
    def loadImages():
        if 'CameraScrollLimiterBlock' in ImageCache: return
        ImageCache[f'CameraScrollLimiterBlock'] = SLib.GetImg(f'camera_scroll_limiter/block.png')
        ImageCache[f'CameraScrollLimiterPermeable'] = SLib.GetImg(f'camera_scroll_limiter/permeable.png')

    def dataChanged(self):
        side = (self.parent.spritedata[2] >> 4) & 0x3
        permeable = ((self.parent.spritedata[2] >> 6) & 0x1) == 0
        distance = (((self.parent.spritedata[2] & 0xF) << 8) | (self.parent.spritedata[3] & 0xFF)) + 1

        s = f'CameraScrollLimiter{"Permeable" if permeable else "Block"}'

        img = QtGui.QPixmap(24 * distance, 24)
        img.fill(QtCore.Qt.transparent)

        for i in range(0, distance, 2):
            image_painter = QtGui.QPainter(img)
            image_painter.drawPixmap(24 * i, 0, ImageCache[s])
            image_painter.end()

        if side == 1: # Bottom
            img = img.transformed(QtGui.QTransform().rotate(180))
            img = img.transformed(QtGui.QTransform().scale(-1, 1))

        elif side == 2: # Left
            img = img.transformed(QtGui.QTransform().rotate(-90))
            img = img.transformed(QtGui.QTransform().scale(1, -1))

        elif side == 3: # Right
            img = img.transformed(QtGui.QTransform().rotate(90))

        self.image = img

        super().dataChanged()
