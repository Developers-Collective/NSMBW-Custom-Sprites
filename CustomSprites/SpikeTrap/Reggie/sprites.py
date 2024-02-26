class SpriteImage_SpikeTrap(SLib.SpriteImage_StaticMultiple):  # XXX
    def __init__(self, parent):
        super().__init__(parent)

    @staticmethod
    def loadImages():
        if 'SpikeTrapS0' in ImageCache: return
        for i in range(16):
            for s in ('s', 'm', 'l'):
                ImageCache[f'SpikeTrap{s.upper()}{i}'] = SLib.GetImg(f'spike_trap_{s}_{i}.png')

    def dataChanged(self):
        color = (self.parent.spritedata[0] & 0xF)
        size = (self.parent.spritedata[0] & 0xC0) >> 6

        rotation = (self.parent.spritedata[1] & 0xFF)
        rot_deg = (rotation / 0xFF) * 360

        s = f'SpikeTrap{("SML"[size]).upper()}{color}'
        image = ImageCache[s] if ImageCache[s] else ImageCache[f'SpikeTrapS0']

        new_image = image.transformed(QtGui.QTransform().rotate(-rot_deg))
        self.image = new_image

        direction = (math.cos(math.radians(-rot_deg + 90)), math.sin(math.radians(-rot_deg + 90)))

        self.offset = (
            8 + (-new_image.width() / 3) + (direction[0] * 8 if size > 0 else 0),
            8 + (-new_image.height() / 3) + (direction[1] * 8 if size > 0 else 0)
        )

        super().dataChanged()