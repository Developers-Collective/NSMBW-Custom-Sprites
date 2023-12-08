class SpriteImage_Goombrat(SLib.SpriteImage_StaticMultiple):  # 488
    @staticmethod
    def loadImages():
        if 'Goombrat0' in ImageCache: return
        for i in range(1):
            ImageCache[f'Goombrat{i}'] = SLib.GetImg(f'goombrat_{i}.png')

    def dataChanged(self):

        color = (self.parent.spritedata[2] & 0xF)

        self.image = ImageCache[f'Goombrat{color}']
        self.offset = (0, -5)

        super().dataChanged()