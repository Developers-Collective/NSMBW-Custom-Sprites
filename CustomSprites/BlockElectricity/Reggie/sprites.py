class SpriteImage_BlockElectricity(SLib.SpriteImage_Static):  # XXX
    def __init__(self, parent):
        super().__init__(parent)

    @staticmethod
    def loadImages():
        if 'BlockElectricity0' in ImageCache: return
        for i in range(16):
            ImageCache[f'BlockElectricity{i}'] = SLib.GetImg(f'block_electricity_{i}.png')
            ImageCache[f'BlockElectricityConstant{i}'] = SLib.GetImg(f'block_electricity_constant_{i}.png')

    def dataChanged(self):
        color = self.parent.spritedata[2] & 0xF
        send_type = (self.parent.spritedata[2] & 0x30) >> 4
        invisible = (self.parent.spritedata[2] & 0x80) >> 7

        s = 'BlockElectricity' + ('Constant' if send_type == 3 else '')

        img = ImageCache[f'{s}{color}'] if ImageCache[f'{s}{color}'] else ImageCache[f'{s}0']

        image = QtGui.QPixmap(24, 24)
        image.fill(QtCore.Qt.transparent)

        image_painter = QtGui.QPainter(image)
        if invisible: image_painter.setOpacity(0.5)
        image_painter.drawPixmap(0, 0, img)
        image_painter.end()

        self.image = image

        self.offset = (0, 0)

        super().dataChanged()