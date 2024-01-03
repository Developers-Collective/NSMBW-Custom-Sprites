class SpriteImage_Komboo(SLib.SpriteImage_Static):  # XXX
    SIZES = ['s', 'm']

    def __init__(self, parent):
        super().__init__(parent)

    @staticmethod
    def loadImages():
        if 'KombooS0_0' in ImageCache: return
        for s in SpriteImage_Komboo.SIZES:
            for i in range(16):
                for j in range(3):
                    ImageCache[f'Komboo{s.upper()}{i}_{j}'] = SLib.GetImg(f'komboo_{s}_{i}_{j}.png')

    def dataChanged(self):
        color = (self.parent.spritedata[2] & 0xF0) >> 4
        pat_color = ((self.parent.spritedata[2] & 0xC) >> 2) % 3
        size = SpriteImage_Komboo.SIZES[(self.parent.spritedata[2] & 0x2) >> 1]

        s = f'Komboo{size.upper()}'
        self.image = ImageCache[f'{s}{color}_{pat_color}'] if ImageCache[f'{s}{color}_{pat_color}'] else ImageCache[f'{s}0_0']

        self.offset = (-2, (-(self.image.height() / 24) * 16) + 16)

        super().dataChanged()



class SpriteImage_KombooSpawner(SLib.SpriteImage_Static):  # YYY
    SIZES = ['s', 'm']

    def __init__(self, parent):
        super().__init__(parent)

    @staticmethod
    def loadImages():
        if 'KombooSpawnerS0' in ImageCache: return
        for s in SpriteImage_KombooSpawner.SIZES:
            for i in range(16):
                ImageCache[f'KombooSpawner{s.upper()}{i}'] = SLib.GetImg(f'komboo_spawner_{s}_{i}.png')

    def dataChanged(self):
        color = (self.parent.spritedata[2] & 0xF0) >> 4
        size = SpriteImage_KombooSpawner.SIZES[(self.parent.spritedata[2] & 0x2) >> 1]

        s = f'KombooSpawner{size.upper()}'

        img = ImageCache[f'{s}{color}'] if ImageCache[f'{s}{color}'] else ImageCache[f'{s}0']

        image = QtGui.QPixmap(img.width(), img.height())
        image.fill(QtCore.Qt.transparent)

        image_painter = QtGui.QPainter(image)
        image_painter.setOpacity(0.625)
        image_painter.drawPixmap(0, 0, img)
        image_painter.end()

        self.image = image

        self.offset = (-24, (-(self.image.height() / 24) * 16) + 16)

        super().dataChanged()
