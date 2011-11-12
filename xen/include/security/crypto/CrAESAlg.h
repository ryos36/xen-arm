/**

 * rijndael-alg-fst.h

 *

 * @version 3.0 (December 2000)

 *

 * Optimised ANSI C code for the Rijndael cipher (now AES)

 *

 * @author Vincent Rijmen <vincent.rijmen@esat.kuleuven.ac.be>

 * @author Antoon Bosselaers <antoon.bosselaers@esat.kuleuven.ac.be>

 * @author Paulo Barreto <paulo.barreto@terra.com.br>

 *

 * This code is hereby placed in the public domain.

 *

 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS ''AS IS'' AND ANY EXPRESS

 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED

 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE

 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE

 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR

 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF

 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR

 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,

 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE

 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,

 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 */

#ifndef _CR_AES_ALGORITHM_H_

#define _CR_AES_ALGORITHM_H_





#define MAXKC	(256/32)

#define MAXKB	(256/8)

#define MAXNR	14



CrINT32 rijndaelKeySetupEnc(CrUINT32 rk[/*4*(Nr + 1)*/], const CrUINT8 cipherKey[], CrINT32 keyBits);

CrINT32 rijndaelKeySetupDec(CrUINT32 rk[/*4*(Nr + 1)*/], const CrUINT8 cipherKey[], CrINT32 keyBits);

void rijndaelEncrypt(const CrUINT32 rk[/*4*(Nr + 1)*/], CrINT32 Nr, const CrUINT8 pt[16], CrUINT8 ct[16]);

void rijndaelDecrypt(const CrUINT32 rk[/*4*(Nr + 1)*/], CrINT32 Nr, const CrUINT8 ct[16], CrUINT8 pt[16]);



#endif // end of #ifndef _CR_AES_ALGORITHM_H_

