class SpriteImage_StarCoin(SLib.SpriteImage_StaticMultiple):  # 32, 155, 389
    def __init__(self, parent):
        super().__init__(parent)

    @staticmethod
    def loadImages():
        if 'StarCoin0' in ImageCache: return
        for i in range(16):
            ImageCache[f'StarCoin{i}'] = SLib.GetImg(f'star_coin{i}.png')

    def dataChanged(self):
        color = self.parent.spritedata[2] & 0xF

        self.image = ImageCache[f'StarCoin{color}']

        self.offset = (0, 3)

        super().dataChanged()


class SpriteImage_FakeStarCoin(SLib.SpriteImage_StaticMultiple):  # 40
    def __init__(self, parent):
        super().__init__(parent)

    @staticmethod
    def loadImages():
        if 'FakeStarCoin0' in ImageCache: return
        for i in range(16):
            ImageCache[f'FakeStarCoin{i}'] = SLib.GetImg(f'starcoin_fake{i}.png')

    def dataChanged(self):
        color = self.parent.spritedata[2] & 0xF

        self.image = ImageCache[f'FakeStarCoin{color}']

        self.offset = (0, 3)

        super().dataChanged()
