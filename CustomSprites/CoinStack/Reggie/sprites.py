class SpriteImage_CoinStack(SLib.SpriteImage_StaticMultiple):  # XXX
    def __init__(self, parent):
        super().__init__(parent)
        self.offset = (0, 0)

    @staticmethod
    def loadImages():
        if 'CoinStack0Rot0' in ImageCache: return
        for i in range(16):
            for j in range(4):
                ImageCache[f'CoinStack{i}Rot{j}'] = SLib.GetImg(f'coin_stack_{i}_rot{j}.png')

    def dataChanged(self):
        nybble5 = (self.parent.spritedata[2] >> 4) & 0xF
        y_rot = (nybble5 >> 2) & 0x3

        color = self.parent.spritedata[2] & 0xF

        s = f'CoinStack{color}Rot{y_rot}'
        self.image = ImageCache[s] if ImageCache[s] else ImageCache[f'CoinStack0_Rot0']

        super().dataChanged()
