describe('window.eparakstshwcrypto', function(){
	mocha.ui('bdd');
	var expect = chai.expect;
	var should = chai.should();

	it('should exist after script inclusion', function () {
		expect(window.eparakstshwcrypto).to.be.a('object'); 
	});
	it('should not have extra properties', function() {
		var okprops = ["use", "debug", "getCertificate", "sign", "NO_IMPLEMENTATION", "USER_CANCEL", "NOT_ALLOWED", "NO_CERTIFICATES", "TECHNICAL_ERROR", "INVALID_ARGUMENT"].sort();
		var props = Object.keys(window.eparakstshwcrypto).sort();
		expect(props).to.have.members(okprops);
	});

	describe('debugging capabilities', function(){
		it('should have debug() method', function() {
			expect(window.eparakstshwcrypto).itself.to.respondTo('debug');
		});
		it('should always succeed', function() {
			return window.eparakstshwcrypto.debug().should.eventually.be.a('string');
		});
		it('should always contain "eparakstshwcrypto"', function() {
			return window.eparakstshwcrypto.debug().should.eventually.contain('eparakstshwcrypto');
		});
	});

	describe('.getCertificate()', function(){
		it('should be rejected without backend', function(){
			return window.eparakstshwcrypto.getCertificate({}).should.be.rejectedWith(Error, window.eparakstshwcrypto.NO_IMPLEMENTATION);
		});
	});
	describe('.sign()', function(){
		it('should be rejected without backend', function(){
			return window.eparakstshwcrypto.sign({}, {}, {}).should.be.rejectedWith(Error, window.eparakstshwcrypto.NO_IMPLEMENTATION);
		});
	});
}); 
