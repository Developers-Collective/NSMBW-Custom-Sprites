class SpriteImage_GreenRing(SLib.SpriteImage):  # XXX
    def __init__(self, parent):
        super().__init__(parent, 1.5)
        self.spritebox.shown = False

        self.aux.append(SLib.AuxiliaryImage(parent, 76, 95))
        self.aux[0].image = ImageCache['GreenRing']
        self.aux[0].setPos(-10, -15)
        self.aux[0].hover = False

        self.dimensions = (-10, -8, 37, 48)

    @staticmethod
    def loadImages():
        SLib.loadIfNotInImageCache('GreenRing', 'green_ring.png')

    def dataChanged(self):
        shifted = self.parent.spritedata[5] & 1
        self.xOffset = -2 if shifted else -10

        super().dataChanged()


class SpriteImage_GreenCoin(SLib.SpriteImage_Static):  # YYY
    def __init__(self, parent):
        super().__init__(
            parent,
            1.65,
            ImageCache['GreenCoin'],
            (-7, 2)
        )

    @staticmethod
    def loadImages():
        SLib.loadIfNotInImageCache('GreenCoin', 'green_coins.png')

